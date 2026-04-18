#include "RadialOrbView.h"
#include "ofMain.h"
#include <algorithm>
#include <cmath>

namespace lqcaa {

namespace {

constexpr float kSectorDeg = 360.f / static_cast<float>(kNumInputChannels);
constexpr float kSectorHalfDeg = kSectorDeg * 0.5f;

float channelVisualLevel(float meterRms){
	const float t = ofMap(meterRms, 0.f, kRmsMapMax, 0.f, 1.f, true);
	return std::sqrt(t);
}

void sectorAnglesRad(int sectorIndex, float & outA0, float & outA1){
	const float centerDeg = -90.f + static_cast<float>(sectorIndex) * kSectorDeg;
	outA0 = ofDegToRad(centerDeg - kSectorHalfDeg);
	outA1 = ofDegToRad(centerDeg + kSectorHalfDeg);
}

void drawAnnulusSector(float cx, float cy, float rInner, float rOuter, float a0, float a1){
	if(rOuter <= rInner + 0.5f){
		return;
	}
	const int steps = 14;
	ofBeginShape();
	for(int i = 0; i <= steps; ++i){
		const float u = static_cast<float>(i) / static_cast<float>(steps);
		const float t = a0 + (a1 - a0) * u;
		ofVertex(cx + rOuter * std::cos(t), cy + rOuter * std::sin(t));
	}
	for(int i = steps; i >= 0; --i){
		const float u = static_cast<float>(i) / static_cast<float>(steps);
		const float t = a0 + (a1 - a0) * u;
		ofVertex(cx + rInner * std::cos(t), cy + rInner * std::sin(t));
	}
	ofEndShape(true);
}

} // namespace

void drawRadialOrb(const std::array<float, kNumInputChannels> & meterDisplay){
	const float w = static_cast<float>(ofGetWidth());
	const float h = static_cast<float>(ofGetHeight());
	const float cx = w * 0.5f;
	const float cy = h * 0.5f + 28.f;
	const float Rmax = std::min(w, h) * 0.42f;
	const float R_in = Rmax * 0.38f;
	const float R_out = Rmax;

	for(int c = 0; c < kNumInputChannels; ++c){
		const float level = channelVisualLevel(meterDisplay[static_cast<size_t>(c)]);
		float a0 = 0.f;
		float a1 = 0.f;
		sectorAnglesRad(c, a0, a1);

		const int alpha = static_cast<int>(ofMap(level, 0.f, 1.f, 15.f, 255.f, true));
		ofFill();
		ofSetColor(100, 200, 255, alpha);
		drawAnnulusSector(cx, cy, R_in, R_out, a0, a1);
	}

	ofNoFill();
	ofSetColor(70, 70, 80, 200);
	ofDrawCircle(cx, cy, R_in);
	ofDrawCircle(cx, cy, R_out);
	for(int k = 0; k < kNumInputChannels; ++k){
		const float ang = ofDegToRad(-90.f - kSectorHalfDeg + static_cast<float>(k) * kSectorDeg);
		ofDrawLine(cx + R_in * std::cos(ang), cy + R_in * std::sin(ang), cx + R_out * std::cos(ang),
				   cy + R_out * std::sin(ang));
	}
}

} // namespace lqcaa
