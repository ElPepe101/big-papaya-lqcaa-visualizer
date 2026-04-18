#pragma once

#include "Constants.h"
#include <array>

namespace lqcaa {

// 4×4 sphere patches: channel index = row(i)*4 + col(j), row 0 = north band, col 0→3 eastward in azimuth.
void drawSphereMeterPatches(const std::array<float, kNumInputChannels> & meterDisplay);
}