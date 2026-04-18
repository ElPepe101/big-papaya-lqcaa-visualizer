#pragma once

#include "Constants.h"
#include <array>
#include <vector>

namespace lqcaa {

void updateMeterDisplayFromInterleaved(const std::vector<float> & interleaved,
									   std::array<float, kNumInputChannels> & meterDisplay);

}
