#pragma once

#include "ofMain.h"

// Fixed +Y up, classic orbit around origin (look-at target). Orthographic projection (OF ofCamera::enableOrtho).
class OrbitOrthoCamera{
 public:
	void setup(float nearClip, float farClip);
	void onDrag(int dx, int dy);
	void applyEye();

	void begin(const ofRectangle & viewport);
	void end();

	/// Camera used for matrices / eye position (orthographic orbit).
	const ofCamera & getCamera() const { return cam_; }

 private:
	ofCamera cam_;
	float nearClip_ = -4000.f;
	float farClip_ = 4000.f;

	float azimuth_ = 0.8f;
	float elevation_ = 0.35f;
	static constexpr float kPitchLimit = 1.48f;
	static constexpr float kDragSensitivity = 0.0055f;
};
