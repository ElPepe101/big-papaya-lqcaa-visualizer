#include "WallGridMeterView.h"
#include "ofMain.h"
#include <algorithm>
#include <cmath>

namespace lqcaa {

namespace {

// World-fixed vertical wall: XY plane at z=0, outward normal +Z (toward default camera hemisphere).
// Row bands: channel c = 0..15 maps to horizontal strip with v in [c/16, (c+1)/16), ch0 = bottom (v=0), ch15 = top (v=1).
constexpr int kChannelRows = 16;
constexpr int kColsPerRow = 64;
constexpr float kWallWidth = 520.f;
constexpr float kWallHeight = 400.f;
constexpr int kMeterLogicalCols = 256;
constexpr float kDbSilenceFloor = -72.f;
constexpr float kDbDisplayMax = 0.f;
constexpr float kNoiseAmp = 14.f;

constexpr float kSpatialRowUniform = 0.f;
constexpr float kSpatialPerCell = 1.f;
constexpr float kEncLinear = 0.f;
constexpr float kEncDb = 1.f;

ofShader gShader;
ofMesh gMesh;
bool gSetupDone = false;
bool gShaderOk = false;

float gSpatialMode = kSpatialPerCell;
float gEncodingMode = kEncDb;

float rmsToLitColumnCount(float rms){
	const float x = std::max(rms, 1e-20f);
	const float dbfs = 20.f * (std::log10(x));
	return ofMap(dbfs, kDbSilenceFloor, kDbDisplayMax, 0.f, static_cast<float>(kMeterLogicalCols), true);
}

void buildWallMesh(){
	const float halfW = kWallWidth * 0.5f;
	const float halfH = kWallHeight * 0.5f;
	const float invM = 1.f / static_cast<float>(kColsPerRow);

	gMesh.clear();
	gMesh.setMode(OF_PRIMITIVE_TRIANGLES);
	gMesh.enableNormals();
	gMesh.enableTextures();
	gMesh.enableColors();

	const glm::vec3 nrm(0.f, 0.f, 1.f);

	for(int row = 0; row < kChannelRows; ++row){
		const float chf = static_cast<float>(row) / 15.f;
		const float y0 = -halfH + kWallHeight * (static_cast<float>(row) / static_cast<float>(kChannelRows));
		const float y1 = -halfH + kWallHeight * (static_cast<float>(row + 1) / static_cast<float>(kChannelRows));

		for(int col = 0; col < kColsPerRow; ++col){
			const float x0 = -halfW + kWallWidth * static_cast<float>(col) * invM;
			const float x1 = -halfW + kWallWidth * static_cast<float>(col + 1) * invM;

			const float u0 = static_cast<float>(col) * invM;
			const float u1 = static_cast<float>(col + 1) * invM;
			const float v0 = static_cast<float>(row) / static_cast<float>(kChannelRows);
			const float v1 = static_cast<float>(row + 1) / static_cast<float>(kChannelRows);

			const int base = static_cast<int>(gMesh.getNumVertices());
			auto pushV = [&](float x, float y, float u, float v){
				gMesh.addVertex(glm::vec3(x, y, 0.f));
				gMesh.addNormal(nrm);
				gMesh.addTexCoord(glm::vec2(u, v));
				gMesh.addColor(ofFloatColor(chf, 0.f, 0.f, 1.f));
			};

			pushV(x0, y0, u0, v0);
			pushV(x1, y0, u1, v0);
			pushV(x1, y1, u1, v1);
			pushV(x0, y1, u0, v1);

			gMesh.addIndex(base + 0);
			gMesh.addIndex(base + 1);
			gMesh.addIndex(base + 2);
			gMesh.addIndex(base + 0);
			gMesh.addIndex(base + 2);
			gMesh.addIndex(base + 3);
		}
	}
}

} // namespace

void setupWallGridMeterView(){
	if(gSetupDone){
		return;
	}
	gSetupDone = true;

	if(!gShader.load("shaders/wall_grid")){
		ofLogError("WallGridMeterView") << "Failed to load shaders/wall_grid.";
		gShaderOk = false;
		return;
	}
	gShaderOk = true;
	buildWallMesh();

	const size_t nVerts = gMesh.getNumVertices();
	const size_t nQuads = static_cast<size_t>(kChannelRows) * static_cast<size_t>(kColsPerRow);
	const size_t tris = nQuads * 2u;
	ofLogNotice("WallGridMeterView") << "Wall grid: XY @ z=0  normal=+Z  ch0=bottom..ch15=top  rows=" << kChannelRows
									  << "  cols=" << kColsPerRow << "  quads=" << nQuads << "  verts=" << nVerts
									  << "  tris=" << tris;
}

void drawWallGridMeter(const ofCamera & cam, const std::array<float, kNumInputChannels> & meterDisplay, int numInputChannels){
	if(!gShaderOk){
		return;
	}

	const int nIn = std::max(0, std::min(numInputChannels, kNumInputChannels));

	float litEnd[16]{};
	float rmsArr[16]{};
	float phase[16]{};
	for(int c = 0; c < kNumInputChannels; ++c){
		const float rms = (c < nIn) ? meterDisplay[static_cast<size_t>(c)] : 0.f;
		rmsArr[c] = rms;
		litEnd[c] = rmsToLitColumnCount(rms);
		phase[c] = static_cast<float>(c) * 0.417f * 6.2831853f;
	}

	const glm::mat4 mvp = cam.getModelViewProjectionMatrix();

	gShader.begin();
	gShader.setUniformMatrix4f("modelViewProjectionMatrix", mvp);
	gShader.setUniform1f("uTime", ofGetElapsedTimef());
	gShader.setUniform1f("uNoiseAmp", kNoiseAmp);
	gShader.setUniform1fv("uPhase", phase, 16);
	gShader.setUniform1f("uSpatialMode", gSpatialMode);
	gShader.setUniform1f("uEncodingMode", gEncodingMode);
	gShader.setUniform1f("uMeterLitCols", static_cast<float>(kMeterLogicalCols));
	gShader.setUniform1f("uRmsMapMax", kRmsMapMax);
	gShader.setUniform1fv("uLitEnd", litEnd, 16);
	gShader.setUniform1fv("uRms", rmsArr, 16);

	gMesh.draw();
	gShader.end();
}

bool handleWallGridMeterKey(int key){
	if(key == '1'){
		gSpatialMode = (gSpatialMode < 0.5f) ? kSpatialPerCell : kSpatialRowUniform;
		ofLogNotice("WallGridMeterView") << "Spatial mode: " << (gSpatialMode > 0.5f ? "per-cell stripe" : "row-uniform");
		return true;
	}
	if(key == '2'){
		gEncodingMode = (gEncodingMode < 0.5f) ? kEncDb : kEncLinear;
		ofLogNotice("WallGridMeterView") << "Encoding: " << (gEncodingMode > 0.5f ? "dB columns" : "linear RMS");
		return true;
	}
	return false;
}

} // namespace lqcaa
