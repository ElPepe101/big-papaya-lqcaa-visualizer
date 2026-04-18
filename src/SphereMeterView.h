#pragma once

#include "Constants.h"
#include <array>

class ofCamera;
class ofRectangle;

namespace lqcaa {

// One-time GL/shader/texture setup for the PBR sphere meter (call from ofApp::setup after GL init).
void setupSphereMeterView();

// Square UV sphere (N×N quads, N multiple of 16): channel scramble per latitude band; RMS→dB→meter displacement (rmsToLitColumnCount).
// `numInputChannels` is the stream channel count.
void drawSphereMeterPBR(const ofCamera & cam, const std::array<float, kNumInputChannels> & meterDisplay, int numInputChannels);

// Draw environment skybox at max depth (call inside same camera as the sphere, after the mesh).
void drawSphereMeterSky();

// 2D UV panel: albedo + grid matching sphere quads (u × v strips); magenta lines = per-band meter litEnd. Call without depth test.
void drawSphereMeterUnfold(const ofRectangle & rect, const std::array<float, kNumInputChannels> & meterDisplay, int numInputChannels);

} // namespace lqcaa
