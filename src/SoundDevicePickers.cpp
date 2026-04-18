#include "SoundDevicePickers.h"

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
