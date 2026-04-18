#include "SphereMeterView.h"
#include "ofMain.h"
#include "ofCubeMap.h"
#include <algorithm>
#include <cmath>

namespace lqcaa {

namespace {

// bin/data/: bread/albedo.png, normal.png, roughness.png, displacement.png, ao.png | shaders/bread_sphere.* | env.hdr

constexpr float kSphereRadius = 200.f;
constexpr float kPolarEps = 0.04f;
// Logical meter length (RMS→dB→columns); geometry can be denser — shader maps via uMeterLitCols.
constexpr int kMeterLogicalCols = 256;
// Azimuth tessellation (higher = smoother ring).
constexpr int kMeterColsGeom = 512;
// Latitude strips per channel band (higher = smoother meridians).
constexpr int kLatSub = 4;
constexpr int kLatitudeBands = kNumInputChannels;

// Max radial push along vertex normal; ~4–5% of radius keeps motion visible without puffing the loaf.
constexpr float kDispAmp = kSphereRadius * 0.045f;
// Height from displacement.png (0.5 = neutral); smaller than kDispAmp so crust reads as surface detail.
constexpr float kDispTexAmp = kSphereRadius * 0.018f;

// dBFS + offset → how many of kMeterCols segments light up (0..256). Tweak kDbDisplayMax vs session level.
constexpr float kDbSilenceFloor = -72.f;
constexpr float kDbDisplayMax = 0.f;

ofShader gShader;
ofTexture gAlbedo;
ofTexture gNormal;
ofTexture gRoughness;
ofTexture gDisplacement;
ofTexture gAo;
ofCubeMap gEnv;
ofMesh gMesh;

bool gSetupDone = false;
bool gShaderOk = false;
bool gEnvOk = false;
bool gLoggedEnvFail = false;
bool gLoggedBreadFallback = false;

/// Map RMS to lit column count in [0, kMeterLogicalCols] using dB scale (absolute, not peak-normalized).
float rmsToLitColumnCount(float rms){
	const float x = std::max(rms, 1e-20f);
	const float dbfs = 20.f * (std::log10(x));
	const float t = ofMap(dbfs, kDbSilenceFloor, kDbDisplayMax, 0.f, static_cast<float>(kMeterLogicalCols), true);
	return t;
}

glm::vec3 spherePoint(float r, float theta, float phi){
	const float st = std::sin(theta);
	return glm::vec3(r * st * std::cos(phi), r * std::cos(theta), r * st * std::sin(phi));
}

void solidRgbTex(ofTexture & tex, const ofColor & c){
	ofPixels px;
	px.allocate(1, 1, OF_IMAGE_COLOR);
	px.setColor(0, 0, c);
	tex.loadData(px);
}

void solidGrayTex(ofTexture & tex, unsigned char g){
	ofPixels px;
	px.allocate(1, 1, OF_IMAGE_GRAYSCALE);
	px.setColor(0, 0, ofColor(g, g, g));
	tex.loadData(px);
}

bool tryLoadBread2d(ofTexture & tex, const std::string & path){
	if(!ofLoadImage(tex, path)){
		return false;
	}
	return tex.isAllocated();
}

void loadBreadTextures(){
	const bool wasArb = ofGetUsingArbTex();
	ofDisableArbTex();

	const std::string base = "bread/";
	const bool a = tryLoadBread2d(gAlbedo, base + "albedo.png");
	const bool n = tryLoadBread2d(gNormal, base + "normal.png");
	const bool r = tryLoadBread2d(gRoughness, base + "roughness.png");
	const bool d = tryLoadBread2d(gDisplacement, base + "displacement.png");
	const bool ao = tryLoadBread2d(gAo, base + "ao.png");
	if(!a || !n || !r || !d || !ao){
		if(!gLoggedBreadFallback){
			ofLogWarning("SphereMeterView") << "Missing bread texture(s); using procedural fallbacks (place PNGs under bin/data/bread/).";
			gLoggedBreadFallback = true;
		}
		if(!a){
			solidRgbTex(gAlbedo, ofColor(210, 175, 130));
		}
		if(!n){
			solidRgbTex(gNormal, ofColor(128, 128, 255));
		}
		if(!r){
			solidGrayTex(gRoughness, 140);
		}
		if(!d){
			solidGrayTex(gDisplacement, 128);
		}
		if(!ao){
			solidGrayTex(gAo, 255);
		}
	}
	gAlbedo.enableMipmap();
	gNormal.enableMipmap();
	gRoughness.enableMipmap();
	gDisplacement.enableMipmap();
	gAo.enableMipmap();
	gAlbedo.generateMipmap();
	gNormal.generateMipmap();
	gRoughness.generateMipmap();
	gDisplacement.generateMipmap();
	gAo.generateMipmap();
	gAlbedo.setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	gNormal.setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	gRoughness.setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	gDisplacement.setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	gAo.setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	if(wasArb){
		ofEnableArbTex();
	}
}

void tryLoadHdri(){
	const char * candidates[] = {"env.hdr", "studio.hdr", "environment.hdr", "hdri/env.hdr"};
	ofCubeMapSettings cs;
	cs.resolution = 512;
	cs.irradianceRes = 32;
	cs.preFilterRes = 128;
	cs.flipVertically = true;

	for(const char * name : candidates){
		cs.filePath = name;
		if(gEnv.load(cs)){
			gEnv.setExposure(0.55f);
			gEnvOk = true;
			ofLogNotice("SphereMeterView") << "HDRI loaded: " << name << "  irradiance=" << gEnv.hasIrradianceMap()
											 << " prefilter=" << gEnv.hasPrefilteredMap() << " cube=" << gEnv.hasCubeMap();
			return;
		}
	}
	if(!gLoggedEnvFail){
		ofLogWarning("SphereMeterView") << "No HDRI in bin/data/ (tried env.hdr, studio.hdr, environment.hdr, hdri/env.hdr).";
		gLoggedEnvFail = true;
	}
	gEnvOk = false;
}

/// Which input channel (0..15) drives this latitude band — coprime stride scrambles neighbors (not 0,1,2…).
constexpr int channelForLatitudeBand(int band){
	return (band * 7 + 5) % kNumInputChannels;
}

// kLatitudeBands × kLatSub latitude strips × kMeterColsGeom azimuth quads; color.r = channel, color.g = column param.
void buildSphereMesh(){
	const float span = static_cast<float>(M_PI) - 2.f * kPolarEps;
	const float twoPi = static_cast<float>(M_PI) * 2.f;
	const int totalLatStrips = kLatitudeBands * kLatSub;
	const float invGeom = 1.f / static_cast<float>(kMeterColsGeom);

	gMesh.clear();
	gMesh.setMode(OF_PRIMITIVE_TRIANGLES);
	gMesh.enableColors();
	gMesh.enableNormals();
	gMesh.enableTextures();

	for(int band = 0; band < kLatitudeBands; ++band){
		const int channel = channelForLatitudeBand(band);
		const float bandT0 = kPolarEps + span * (static_cast<float>(band) / static_cast<float>(kLatitudeBands));
		const float bandT1 = kPolarEps + span * (static_cast<float>(band + 1) / static_cast<float>(kLatitudeBands));
		// Golden-angle azimuth offset so channel strips don’t share one meridional seam.
		const float phiTwist = static_cast<float>(band) * 2.39996322972865332f;

		for(int sub = 0; sub < kLatSub; ++sub){
			const float st0 = static_cast<float>(sub) / static_cast<float>(kLatSub);
			const float st1 = static_cast<float>(sub + 1) / static_cast<float>(kLatSub);
			const float t0 = glm::mix(bandT0, bandT1, st0);
			const float t1 = glm::mix(bandT0, bandT1, st1);

			const int stripIndex = band * kLatSub + sub;
			const float vc = (static_cast<float>(stripIndex) + 0.5f) / static_cast<float>(totalLatStrips);

			for(int j = 0; j < kMeterColsGeom; ++j){
				const float phi0 = twoPi * (static_cast<float>(j) * invGeom) + phiTwist;
				const float phi1 = twoPi * (static_cast<float>(j + 1) * invGeom) + phiTwist;

				const glm::vec3 v00 = spherePoint(kSphereRadius, t0, phi0);
				const glm::vec3 v10 = spherePoint(kSphereRadius, t0, phi1);
				const glm::vec3 v11 = spherePoint(kSphereRadius, t1, phi1);
				const glm::vec3 v01 = spherePoint(kSphereRadius, t1, phi0);

				const glm::vec3 n00 = glm::normalize(v00);
				const glm::vec3 n10 = glm::normalize(v10);
				const glm::vec3 n11 = glm::normalize(v11);
				const glm::vec3 n01 = glm::normalize(v01);

				const float u0 = std::fmod(phi0, twoPi) / twoPi;
				const float u1 = std::fmod(phi1, twoPi) / twoPi;

				const float colNorm = (static_cast<float>(j) + 0.5f) * invGeom;

				auto pushV = [&](const glm::vec3 & p, const glm::vec3 & n, float u){
					gMesh.addVertex(p);
					gMesh.addNormal(n);
					gMesh.addTexCoord(glm::vec2(u, vc));
					gMesh.addColor(ofFloatColor(static_cast<float>(channel), colNorm, 0.f, 1.f));
				};

				const int base = static_cast<int>(gMesh.getNumVertices());
				pushV(v00, n00, u0);
				pushV(v10, n10, u1);
				pushV(v11, n11, u1);
				pushV(v01, n01, u0);

				gMesh.addIndex(base + 0);
				gMesh.addIndex(base + 1);
				gMesh.addIndex(base + 2);
				gMesh.addIndex(base + 0);
				gMesh.addIndex(base + 2);
				gMesh.addIndex(base + 3);
			}
		}
	}
}

// Same (u,v) as sphere mesh texcoords: u = azimuth wrap, v = (stripIndex+0.5)/(bands*latSub).
void drawSphereMeterUnfoldBody(const ofRectangle & rect, const std::array<float, kNumInputChannels> & meterDisplay, int numInputChannels){
	if(!gAlbedo.isAllocated() || rect.width < 8.f || rect.height < 8.f){
		return;
	}
	const int nIn = std::max(0, std::min(numInputChannels, kNumInputChannels));
	float litEnd[16]{};
	for(int c = 0; c < kNumInputChannels; ++c){
		const float rms = (c < nIn) ? meterDisplay[static_cast<size_t>(c)] : 0.f;
		litEnd[c] = rmsToLitColumnCount(rms);
	}
	const int totalLatStrips = kLatitudeBands * kLatSub;
	const int winH = ofGetHeight();
	const int sx = static_cast<int>(rect.x);
	const int sw = static_cast<int>(rect.width);
	const int sh = static_cast<int>(rect.height);
	const int sy = winH - static_cast<int>(rect.y + rect.height);

	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	glEnable(GL_SCISSOR_TEST);
	glScissor(sx, sy, sw, sh);

	ofPushMatrix();
	ofTranslate(rect.x, rect.y);
	ofSetColor(255);
	gAlbedo.draw(0.f, 0.f, rect.width, rect.height);

	ofSetLineWidth(1.f);
	ofNoFill();
	ofSetColor(255, 255, 255, 40);
	for(int j = 0; j <= kMeterColsGeom; ++j){
		const float x = rect.width * (static_cast<float>(j) / static_cast<float>(kMeterColsGeom));
		ofDrawLine(x, 0.f, x, rect.height);
	}

	ofSetColor(255, 210, 90, 130);
	const int majorStep = kMeterColsGeom / 4;
	for(int j = 0; j <= kMeterColsGeom; j += majorStep){
		const float x = rect.width * (static_cast<float>(j) / static_cast<float>(kMeterColsGeom));
		ofSetLineWidth(2.f);
		ofDrawLine(x, 0.f, x, rect.height);
		ofSetLineWidth(1.f);
	}

	ofSetColor(190, 210, 255, 50);
	for(int s = 0; s <= totalLatStrips; ++s){
		const float y = rect.height * (static_cast<float>(s) / static_cast<float>(totalLatStrips));
		ofDrawLine(0.f, y, rect.width, y);
	}

	ofSetColor(90, 255, 170, 160);
	for(int band = 0; band <= kLatitudeBands; ++band){
		const float y = rect.height * (static_cast<float>(band * kLatSub) / static_cast<float>(totalLatStrips));
		ofSetLineWidth(2.f);
		ofDrawLine(0.f, y, rect.width, y);
		ofSetLineWidth(1.f);
	}

	ofSetColor(255, 60, 200, 210);
	for(int band = 0; band < kLatitudeBands; ++band){
		const int ch = channelForLatitudeBand(band);
		const float u = std::clamp(litEnd[ch] / static_cast<float>(kMeterLogicalCols), 0.f, 1.f);
		const float x = rect.width * u;
		const float y0 = rect.height * (static_cast<float>(band * kLatSub) / static_cast<float>(totalLatStrips));
		const float y1 = rect.height * (static_cast<float>((band + 1) * kLatSub) / static_cast<float>(totalLatStrips));
		ofSetLineWidth(3.f);
		ofDrawLine(x, y0, x, y1);
		ofSetLineWidth(1.f);
	}

	ofPopMatrix();
	glDisable(GL_SCISSOR_TEST);

	ofSetColor(255, 255, 255, 220);
	ofNoFill();
	ofDrawRectangle(rect);
	ofDrawBitmapString("UV = sphere texcoords (u cols=" + ofToString(kMeterColsGeom) + "  v strips=" + ofToString(totalLatStrips) + ")  orange=u/4  green=ch band  magenta=meter", rect.x + 4.f, rect.y + rect.height + 14.f);

	ofPopStyle();
}

} // namespace

void setupSphereMeterView(){
	if(gSetupDone){
		return;
	}
	gSetupDone = true;

	if(!gShader.load("shaders/bread_sphere")){
		ofLogError("SphereMeterView") << "Failed to load shaders/bread_sphere.";
		gShaderOk = false;
		return;
	}
	gShaderOk = true;

	loadBreadTextures();
	tryLoadHdri();
	buildSphereMesh();

	ofLogNotice("SphereMeterView") << "PBR: " << (gLoggedBreadFallback ? "partial fallbacks" : "bread/*.png OK") << " | IBL env=" << gEnvOk;
}

void drawSphereMeterPBR(const ofCamera & cam, const std::array<float, kNumInputChannels> & meterDisplay, int numInputChannels){
	if(!gShaderOk){
		return;
	}

	const int nIn = std::max(0, std::min(numInputChannels, kNumInputChannels));

	float litEnd[16]{};
	for(int c = 0; c < kNumInputChannels; ++c){
		const float rms = (c < nIn) ? meterDisplay[static_cast<size_t>(c)] : 0.f;
		litEnd[c] = rmsToLitColumnCount(rms);
	}

	const bool irr = gEnvOk && gEnv.hasIrradianceMap();
	const bool pre = gEnvOk && gEnv.hasPrefilteredMap();

	const glm::mat4 mv = cam.getModelViewMatrix();
	const glm::mat4 proj = cam.getProjectionMatrix();
	const glm::mat4 mvp = proj * mv;
	const glm::vec3 eye = cam.getPosition();
	// Directions from surface toward lights (world space; sphere at origin).
	const glm::vec3 Lkey = glm::normalize(glm::vec3(0.52f, 0.78f, 0.35f));
	const glm::vec3 Lfill = glm::normalize(glm::vec3(-0.82f, 0.12f, -0.56f));
	const glm::vec3 Laccent = glm::normalize(glm::vec3(-0.25f, -0.88f, 0.42f));

	gShader.begin();
	gShader.setUniformMatrix4f("modelMatrix", glm::mat4(1.f));
	gShader.setUniformMatrix4f("modelViewProjectionMatrix", mvp);
	gShader.setUniform3f("uCameraPos", eye.x, eye.y, eye.z);
	gShader.setUniform3f("uLKey", Lkey.x, Lkey.y, Lkey.z);
	gShader.setUniform3f("uLFill", Lfill.x, Lfill.y, Lfill.z);
	gShader.setUniform3f("uLAccent", Laccent.x, Laccent.y, Laccent.z);
	gShader.setUniform1f("uUseEnvDiffuse", irr ? 1.f : 0.f);
	gShader.setUniform1f("uUseEnvSpec", pre ? 1.f : 0.f);
	gShader.setUniform1f("uEnvMaxMips", 5.f);
	gShader.setUniform1f("uDispAmp", kDispAmp);
	gShader.setUniform1f("uDispTexScale", kDispTexAmp);
	gShader.setUniform1f("uMeterLitCols", static_cast<float>(kMeterLogicalCols));
	gShader.setUniform1fv("uLitEnd", litEnd, 16);

	gAlbedo.bind(0);
	gNormal.bind(1);
	gRoughness.bind(2);
	gDisplacement.bind(5);
	gAo.bind(6);
	gShader.setUniform1i("uAlbedo", 0);
	gShader.setUniform1i("uNormal", 1);
	gShader.setUniform1i("uRoughness", 2);
	gShader.setUniform1i("uDisplacement", 5);
	gShader.setUniform1i("uAo", 6);

	if(irr){
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, gEnv.getIrradianceMapId());
		gShader.setUniform1i("uIrradiance", 3);
	}
	if(pre){
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, gEnv.getPrefilterMapId());
		gShader.setUniform1i("uPrefilter", 4);
	}

	gMesh.draw();

	gShader.end();

	gAlbedo.unbind(0);
	gNormal.unbind(1);
	gRoughness.unbind(2);
	gDisplacement.unbind(5);
	gAo.unbind(6);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE0);
}

void drawSphereMeterSky(){
	if(!gEnvOk || !gEnv.hasCubeMap()){
		return;
	}
	gEnv.drawCubeMap();
}

void drawSphereMeterUnfold(const ofRectangle & rect, const std::array<float, kNumInputChannels> & meterDisplay, int numInputChannels){
	if(!gSetupDone || !gAlbedo.isAllocated()){
		return;
	}
	drawSphereMeterUnfoldBody(rect, meterDisplay, numInputChannels);
}

} // namespace lqcaa
