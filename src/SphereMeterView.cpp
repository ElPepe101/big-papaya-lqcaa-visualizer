#include "SphereMeterView.h"
#include "ofMain.h"
#include <cmath>

namespace lqcaa {

namespace {

constexpr float kSphereRadius = 200.f;
constexpr int kGrid = 4;
constexpr float kPolarEps = 0.04f;

float channelVisualLevel(float meterRms){
	const float t = ofMap(meterRms, 0.f, kRmsMapMax, 0.f, 1.f, true);
	return std::sqrt(t);
}

glm::vec3 spherePoint(float r, float theta, float phi){
	// theta colatitude from +Y (north); phi azimuth (XZ plane).
	const float st = std::sin(theta);
	return glm::vec3(r * st * std::cos(phi), r * std::cos(theta), r * st * std::sin(phi));
}

static float hueForChannel(int c){
	const float golden = 137.508f;
	return std::fmod(static_cast<float>(c) * golden, 255.f);
}

void addTri(ofMesh & mesh, const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c, const ofColor & col){
	mesh.addVertex(a);
	mesh.addColor(col);
	mesh.addVertex(b);
	mesh.addColor(col);
	mesh.addVertex(c);
	mesh.addColor(col);
}

} // namespace

// Patch grid: row 0 = north polar band … row 3 = south; col 0…3 = azimuth west→east.
// channelIndex = row * 4 + col (ch1 = row 0, col 0).
void drawSphereMeterPatches(const std::array<float, kNumInputChannels> & meterDisplay){
	const float span = static_cast<float>(M_PI) - 2.f * kPolarEps;
	const float dTheta = span / static_cast<float>(kGrid);
	const float dPhi = static_cast<float>(M_PI) * 2.f / static_cast<float>(kGrid);

	ofMesh mesh;
	mesh.enableColors();
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);

	for(int row = 0; row < kGrid; ++row){
		for(int col = 0; col < kGrid; ++col){
			const int ch = row * kGrid + col;
			const float level = channelVisualLevel(meterDisplay[static_cast<size_t>(ch)]);
			const float bright = ofMap(level, 0.f, 1.f, 28.f, 255.f, true);
			const float hue = hueForChannel(ch);
			ofColor fill;
			fill.setHsb(static_cast<unsigned char>(hue), 235, static_cast<unsigned char>(bright), 255);

			const float t0 = kPolarEps + static_cast<float>(row) * dTheta;
			const float t1 = t0 + dTheta;
			const float p0 = static_cast<float>(col) * dPhi;
			const float p1 = p0 + dPhi;

			const glm::vec3 v00 = spherePoint(kSphereRadius, t0, p0);
			const glm::vec3 v01 = spherePoint(kSphereRadius, t0, p1);
			const glm::vec3 v11 = spherePoint(kSphereRadius, t1, p1);
			const glm::vec3 v10 = spherePoint(kSphereRadius, t1, p0);

			addTri(mesh, v00, v01, v11, fill);
			addTri(mesh, v00, v11, v10, fill);
		}
	}

	ofSetColor(255);
	mesh.draw();
}

} // namespace lqcaa
