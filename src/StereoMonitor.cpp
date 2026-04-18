#include "StereoMonitor.h"
#include <algorithm>

namespace lqcaa {

bool fillStereoMonitorFromInterleaved(const std::vector<float> & lastInputInterleaved,
									  float * output,
									  size_t nFrames,
									  size_t outCh,
									  size_t inCh){
	const size_t availFrames = lastInputInterleaved.size() / inCh;
	const size_t useFrames = std::min(nFrames, availFrames);
	const int chSum = static_cast<int>(std::min(static_cast<size_t>(kNumInputChannels), inCh));

	bool clipped = false;
	for(size_t f = 0; f < useFrames; ++f){
		float sum = 0.f;
		for(int c = 0; c < chSum; ++c){
			sum += lastInputInterleaved[f * inCh + static_cast<size_t>(c)];
		}
		float m = sum * kMonitorGain;
		if(m > 1.f){
			m = 1.f;
			clipped = true;
		}else if(m < -1.f){
			m = -1.f;
			clipped = true;
		}
		output[f * outCh + 0] = m;
		output[f * outCh + 1] = m;
	}
	for(size_t f = useFrames; f < nFrames; ++f){
		output[f * outCh + 0] = 0.f;
		output[f * outCh + 1] = 0.f;
	}
	return clipped;
}

} // namespace lqcaa
