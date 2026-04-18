#include "ofApp.h"
#include "MeterBlocks.h"
#include "SoundDevicePickers.h"
#include "SphereMeterView.h"
#include "StereoMonitor.h"
#include <algorithm>

using namespace lqcaa;

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofBackground(54, 54, 54);

	orbitCam_.setup(-4000.f, 4000.f);

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
	updateMeterDisplayFromInterleaved(lastInputInterleaved, meterDisplay);
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(225);
	ofDrawBitmapString("LQCAA — 16ch RMS meters + stereo monitor (sum * " + ofToString(kMonitorGain, 2) + ")", 12, 20);

	std::string dbg = "RMS  sampleRate=" + (audioSetupOk ? ofToString(actualSampleRate) : std::string("—"));
	dbg += "  bufferSize=" + (audioSetupOk ? ofToString(actualBufferSize) : std::string("—"));
	dbg += "  tick=" + ofToString(soundStream.getTickCount());
	if(monitorClipWarning){
		dbg += "  [CLIP]";
	}
	ofDrawBitmapString(dbg, 12, 38);

	ofEnableDepthTest();
	const ofRectangle vp(0, 0, ofGetWidth(), ofGetHeight());
	orbitCam_.begin(vp);
	lqcaa::drawSphereMeterPatches(meterDisplay);
	orbitCam_.end();
	ofDisableDepthTest();

	ofSetColor(200);
	ofDrawBitmapString("Drag sphere view: left mouse orbit (ortho, +Y up).", 12, 56);

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

	float * buf = output.getBuffer().data();
	const size_t inCh = static_cast<size_t>(kNumInputChannels);
	monitorClipWarning = fillStereoMonitorFromInterleaved(lastInputInterleaved, buf, nFrames, outCh, inCh);
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
	if(!orbDragging_ || button != 0){
		return;
	}
	const int dx = x - dragLast_.x;
	const int dy = y - dragLast_.y;
	dragLast_ = glm::ivec2(x, y);
	orbitCam_.onDrag(dx, dy);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if(button == 0){
		orbDragging_ = true;
		dragLast_ = glm::ivec2(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if(button == 0){
		orbDragging_ = false;
	}
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
