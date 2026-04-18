#pragma once

#include "ofMain.h"
#include <array>
#include <mutex>
#include <vector>

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
} // namespace lqcaa

class ofApp : public ofBaseApp{
 public:
	void setup() override;
	void update() override;
	void draw() override;
	void exit() override;

	void audioIn(ofSoundBuffer & input) override;
	void audioOut(ofSoundBuffer & output) override;

	void keyPressed(int key) override;
	void keyReleased(int key) override;
	void mouseMoved(int x, int y) override;
	void mouseDragged(int x, int y, int button) override;
	void mousePressed(int x, int y, int button) override;
	void mouseReleased(int x, int y, int button) override;
	void mouseEntered(int x, int y) override;
	void mouseExited(int x, int y) override;
	void windowResized(int w, int h) override;
	void dragEvent(ofDragInfo dragInfo) override;
	void gotMessage(ofMessage msg) override;

	ofSoundStream soundStream;

 private:
	std::mutex audioMutex;
	std::vector<float> lastInputInterleaved;
	std::array<float, lqcaa::kNumInputChannels> peakDisplay{};

	bool audioSetupOk = false;
	int actualSampleRate = 0;
	int actualBufferSize = 0;
	bool monitorClipWarning = false;
};
