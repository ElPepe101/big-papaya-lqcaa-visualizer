#include "MeterBlocks.h"
#include <cmath>

namespace lqcaa {

void updateMeterDisplayFromInterleaved(const std::vector<float> & interleaved,
									   std::array<float, kNumInputChannels> & meterDisplay){
	if(interleaved.empty()){
		return;
	}
	const size_t nFrames = interleaved.size() / static_cast<size_t>(kNumInputChannels);
	if(nFrames == 0){
		return;
	}

	const size_t ch = static_cast<size_t>(kNumInputChannels);
	std::array<double, kNumInputChannels> sumSq{};
	sumSq.fill(0.0);
	for(size_t f = 0; f < nFrames; ++f){
		for(int c = 0; c < kNumInputChannels; ++c){
			const float s = interleaved[f * ch + static_cast<size_t>(c)];
			sumSq[static_cast<size_t>(c)] += static_cast<double>(s) * static_cast<double>(s);
		}
	}
	const double n = static_cast<double>(nFrames);
	for(int c = 0; c < kNumInputChannels; ++c){
		const size_t ci = static_cast<size_t>(c);
		const float rms = static_cast<float>(std::sqrt(sumSq[ci] / n));
		meterDisplay[ci] = meterDisplay[ci] * (1.f - kMeterEmaBlend) + rms * kMeterEmaBlend;
	}
}

} // namespace lqcaa
