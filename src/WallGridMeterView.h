#pragma once

#include "Constants.h"
#include <array>

class ofCamera;

namespace lqcaa {

// World-fixed XY wall at z=0, normal +Z. 16 horizontal row bands (ch0 = bottom row … ch15 = top), M columns per row.
// Keys: [1] spatial row-uniform vs per-cell modulation | [2] encoding linear-RMS vs dB column mapping (see ofApp help line).
void setupWallGridMeterView();

void drawWallGridMeter(const ofCamera & cam, const std::array<float, kNumInputChannels> & meterDisplay, int numInputChannels);

/// Handle mode keys; returns true if key was consumed (1/2).
bool handleWallGridMeterKey(int key);

} // namespace lqcaa
