#include "ofApp.h"
#include <algorithm>
#include <cmath>

using namespace lqcaa;

namespace {

bool pickDefaultStereoOut(ofSoundStream & stream, ofSoundDevice & out){
	auto devs = stream.getDeviceList();
	for(const auto & d: devs){
		if(d.outputChannels >= 2 && d.isDefaultOutput){
			out = d;
			return true;
		}
	}
	for(const auto & d: devs){
		if(d.outputChannels >= 2){
			out = d;
			return true;
		}
	}
	return false;
}

bool pickBlackHole16In(ofSoundStream & stream, ofSoundDevice & in){
	auto exact = stream.getMatchingDevices("BlackHole", 16, 0);
	if(!exact.empty()){
		in = exact[0];
		return in.inputChannels >= 16;
	}
	auto loose = stream.getMatchingDevices("BlackHole");
	if(!loose.empty()){
		in = loose[0];
		if(in.inputChannels < 16){
			ofLogWarning("ofApp") << "BlackHole device \"" << in.name << "\" reports " << in.inputChannels
								  << " inputs; 16 required for this build.";
			return false;
		}
		return true;
	}
	return false;
}

} // namespace

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofBackground(54, 54, 54);

	soundStream.printDeviceList();

	ofSoundStreamSettings settings;
	settings.setInListener(this);
	settings.setOutListener(this);
	settings.sampleRate = kSampleRate;
	settings.bufferSize = kBufferSize;
	settings.numBuffers = 4;
	settings.numInputChannels = kNumInputChannels;
	settings.numOutputChannels = kNumOutputChannels;

	ofSoundDevice inDev;
	ofSoundDevice outDev;
	bool haveIn = pickBlackHole16In(soundStream, inDev);
	bool haveOut = pickDefaultStereoOut(soundStream, outDev);

	if(haveIn){
		settings.setInDevice(inDev);
		ofLogNotice("ofApp") << "Input device: " << inDev.name << " (inputs=" << inDev.inputChannels << ")";
	}else{
		ofLogError("ofApp") << "No BlackHole device with 16 inputs found. Check Audio MIDI Setup / BlackHole 16ch.";
		auto defs = soundStream.getMatchingDevices("default");
		if(!defs.empty()){
			settings.setInDevice(defs[0]);
			ofLogWarning("ofApp") << "Falling back to default input: " << defs[0].name;
		}
	}

	if(haveOut){
		settings.setOutDevice(outDev);
		ofLogNotice("ofApp") << "Output device: " << outDev.name << " (outputs=" << outDev.outputChannels << ")";
	}else{
		ofLogError("ofApp") << "No stereo output device found.";
	}

	if(!soundStream.setup(settings)){
		ofLogError("ofApp") << "soundStream.setup failed.";
		audioSetupOk = false;
		return;
	}

	audioSetupOk = true;
	actualSampleRate = soundStream.getSampleRate();
	actualBufferSize = soundStream.getBufferSize();

	ofLogNotice("ofApp") << "Audio running: sampleRate=" << actualSampleRate << " bufferSize=" << actualBufferSize
						 << " inCh=" << soundStream.getNumInputChannels() << " outCh=" << soundStream.getNumOutputChannels();

	if(soundStream.getNumInputChannels() != kNumInputChannels){
		ofLogError("ofApp") << "Expected " << kNumInputChannels << " input channels; got " << soundStream.getNumInputChannels()
							<< ". Fix device or aggregate in Audio MIDI Setup.";
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	std::lock_guard<std::mutex> lock(audioMutex);
	if(lastInputInterleaved.empty()){
		return;
	}
	const size_t nFrames = lastInputInterleaved.size() / static_cast<size_t>(kNumInputChannels);
	if(nFrames == 0){
		return;
	}

	std::array<float, kNumInputChannels> framePeak{};
	framePeak.fill(0.f);
	for(size_t f = 0; f < nFrames; ++f){
		for(int c = 0; c < kNumInputChannels; ++c){
			const float v = std::abs(lastInputInterleaved[f * static_cast<size_t>(kNumInputChannels) + static_cast<size_t>(c)]);
			framePeak[static_cast<size_t>(c)] = std::max(framePeak[static_cast<size_t>(c)], v);
		}
	}
	for(int c = 0; c < kNumInputChannels; ++c){
		const size_t ci = static_cast<size_t>(c);
		peakDisplay[ci] = std::max(framePeak[ci], peakDisplay[ci] * 0.92f);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(225);
	ofDrawBitmapString("LQCAA — 16ch meters + stereo monitor (sum * " + ofToString(kMonitorGain, 2) + ")", 12, 20);

	std::string dbg = "sampleRate=" + (audioSetupOk ? ofToString(actualSampleRate) : std::string("—"));
	dbg += "  bufferSize=" + (audioSetupOk ? ofToString(actualBufferSize) : std::string("—"));
	dbg += "  tick=" + ofToString(soundStream.getTickCount());
	if(monitorClipWarning){
		dbg += "  [CLIP]";
	}
	ofDrawBitmapString(dbg, 12, 38);

	const float margin = 12.f;
	const float cellW = (ofGetWidth() - margin * 2.f) / 4.f;
	const float cellH = (ofGetHeight() - 80.f - margin) / 4.f;
	const float maxBar = cellH - 24.f;

	for(int c = 0; c < kNumInputChannels; ++c){
		const int col = c % 4;
		const int row = c / 4;
		const float x0 = margin + static_cast<float>(col) * cellW;
		const float y0 = 56.f + static_cast<float>(row) * cellH;
		ofSetColor(180);
		ofDrawBitmapString("ch " + ofToString(c + 1), x0 + 4.f, y0 + 14.f);
		ofNoFill();
		ofSetColor(120);
		ofDrawRectangle(x0 + 2.f, y0 + 18.f, cellW - 8.f, maxBar);
		const float h = peakDisplay[static_cast<size_t>(c)] * maxBar;
		ofFill();
		ofSetColor(100, 200, 255);
		ofDrawRectangle(x0 + 4.f, y0 + 18.f + (maxBar - h), cellW - 12.f, h);
	}

	ofSetColor(180);
	ofDrawBitmapString("Route Ableton → BlackHole 16ch; manual QA: verify each ch + headphone monitor.", 12, static_cast<float>(ofGetHeight()) - 10.f);
}

//--------------------------------------------------------------
void ofApp::exit(){
	soundStream.close();
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input){
	const size_t n = input.size();
	if(n == 0){
		return;
	}
	std::lock_guard<std::mutex> lock(audioMutex);
	lastInputInterleaved.resize(n);
	const float * src = input.getBuffer().data();
	std::copy(src, src + n, lastInputInterleaved.begin());
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer & output){
	const size_t nFrames = output.getNumFrames();
	const size_t outCh = output.getNumChannels();
	if(outCh < 2 || nFrames == 0){
		return;
	}

	std::lock_guard<std::mutex> lock(audioMutex);
	if(lastInputInterleaved.empty()){
		output.set(0.f);
		return;
	}

	const size_t inCh = static_cast<size_t>(kNumInputChannels);
	const size_t availFrames = lastInputInterleaved.size() / inCh;
	const size_t useFrames = std::min(nFrames, availFrames);

	bool clipped = false;
	for(size_t f = 0; f < useFrames; ++f){
		float sum = 0.f;
		for(int c = 0; c < kNumInputChannels; ++c){
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
	monitorClipWarning = clipped;
	for(size_t f = useFrames; f < nFrames; ++f){
		output[f * outCh + 0] = 0.f;
		output[f * outCh + 1] = 0.f;
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
}
