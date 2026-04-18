#pragma once

#include "Constants.h"
#include <array>
#include <vector>

namespace lqcaa {

// `numChannelsInBuffer` must match the stream layout (e.g. soundStream.getNumInputChannels()).
void updateMeterDisplayFromInterleaved(const std::vector<float> & interleaved,
									   std::array<float, kNumInputChannels> & meterDisplay,
									   int numChannelsInBuffer);

}
