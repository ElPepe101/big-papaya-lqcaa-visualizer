#pragma once

#include "Constants.h"
#include "OrbitOrthoCamera.h"
#include "ofMain.h"
#include <array>
#include <mutex>
#include <vector>

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
	// Smoothed RMS-driven levels for visualization (0..~kRmsMapMax typical).
	std::array<float, lqcaa::kNumInputChannels> meterDisplay{};

	bool audioSetupOk = false;
	int actualSampleRate = 0;
	int actualBufferSize = 0;
	bool monitorClipWarning = false;

	OrbitOrthoCamera orbitCam_;
	glm::ivec2 dragLast_{0, 0};
	bool orbDragging_ = false;
};
