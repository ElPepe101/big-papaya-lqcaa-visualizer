#include "MeterBlocks.h"
#include <cmath>

namespace lqcaa {

void updateMeterDisplayFromInterleaved(const std::vector<float> & interleaved,
									   std::array<float, kNumInputChannels> & meterDisplay,
									   int numChannelsInBuffer){
	if(interleaved.empty() || numChannelsInBuffer <= 0){
		return;
	}
	const size_t chBuf = static_cast<size_t>(numChannelsInBuffer);
	const size_t nFrames = interleaved.size() / chBuf;
	if(nFrames == 0){
		return;
	}

	const int chActive = std::min(numChannelsInBuffer, kNumInputChannels);
	std::array<double, kNumInputChannels> sumSq{};
	sumSq.fill(0.0);
	for(size_t f = 0; f < nFrames; ++f){
		for(int c = 0; c < chActive; ++c){
			const float s = interleaved[f * chBuf + static_cast<size_t>(c)];
			sumSq[static_cast<size_t>(c)] += static_cast<double>(s) * static_cast<double>(s);
		}
	}
	const double n = static_cast<double>(nFrames);
	for(int c = 0; c < chActive; ++c){
		const size_t ci = static_cast<size_t>(c);
		const float rms = static_cast<float>(std::sqrt(sumSq[ci] / n));
		meterDisplay[ci] = meterDisplay[ci] * (1.f - kMeterEmaBlend) + rms * kMeterEmaBlend;
	}
	for(int c = chActive; c < kNumInputChannels; ++c){
		const size_t ci = static_cast<size_t>(c);
		meterDisplay[ci] *= (1.f - kMeterEmaBlend);
	}
}

} // namespace lqcaa
