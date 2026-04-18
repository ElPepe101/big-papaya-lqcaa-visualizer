#pragma once

namespace lqcaa {
// Audio I/O layout: BlackHole 16ch → app; stereo monitor out (see design.md).
constexpr int kNumInputChannels = 16;
constexpr int kNumOutputChannels = 2;
// Initial buffer size request; actual size may differ — log after setup.
constexpr int kBufferSize = 512;
// Match Ableton / BlackHole session (44100 is also common).
constexpr unsigned int kSampleRate = 48000;
// Sum of 16 channels: scale down before hard clip (~-14 dB vs full-scale sum).
constexpr float kMonitorGain = 0.2f;
// Blend new block RMS into displayed level each update (higher = snappier).
constexpr float kMeterEmaBlend = 0.22f;
// Map RMS [0, kRmsMapMax] to full bar; typical line-level material sits below ~0.3 RMS.
constexpr float kRmsMapMax = 0.28f;
} // namespace lqcaa
