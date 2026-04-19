#include "OrbitOrthoCamera.h"
#include <cmath>

void OrbitOrthoCamera::setup(float nearClip, float farClip){
	nearClip_ = nearClip;
	farClip_ = farClip;
	cam_.setNearClip(nearClip);
	cam_.setFarClip(farClip);
	// Perspective orbit for plane meter (infinite-plane-meter-perspective-pbr).
	cam_.setupPerspective(false, 55.f, nearClip, farClip);
	cam_.disableOrtho();
	cam_.setVFlip(false);
}

void OrbitOrthoCamera::onDrag(int dx, int dy){
	azimuth_ += static_cast<float>(dx) * kDragSensitivity;
	elevation_ -= static_cast<float>(dy) * kDragSensitivity;
	if(elevation_ > kPitchLimit){
		elevation_ = kPitchLimit;
	}
	if(elevation_ < -kPitchLimit){
		elevation_ = -kPitchLimit;
	}
}

void OrbitOrthoCamera::applyEye(){
	const float cp = std::cos(elevation_);
	const float sp = std::sin(elevation_);
	const float sa = std::sin(azimuth_);
	const float ca = std::cos(azimuth_);
	// Wall meter (~520×400): closer eye reads grid + noise; world-wall-noise-grid-meter.
	const float distance = 720.f;
	const glm::vec3 pos(distance * cp * sa, distance * sp, distance * cp * ca);
	cam_.setPosition(pos);
	cam_.lookAt(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
}

void OrbitOrthoCamera::begin(const ofRectangle & viewport){
	applyEye();
	cam_.begin(viewport);
}

void OrbitOrthoCamera::end(){
	cam_.end();
}
