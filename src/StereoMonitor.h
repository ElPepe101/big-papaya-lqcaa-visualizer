#pragma once

#include "Constants.h"
#include <cstddef>
#include <vector>

namespace lqcaa {

// Writes summed mono-to-stereo monitor into interleaved stereo output; returns true if any sample clipped.
bool fillStereoMonitorFromInterleaved(const std::vector<float> & lastInputInterleaved,
									  float * output,
									  size_t nFrames,
									  size_t outCh,
									  size_t inCh);

}
