#pragma once

#include "ofMain.h"

// Fixed +Y up, classic orbit around origin (look-at target). Perspective projection (FOV 55° default in setup).
class OrbitOrthoCamera{
 public:
	/// Perspective clip planes; suitable for large ground plane + sky (e.g. near 15, far 25000).
	void setup(float nearClip, float farClip);
	void onDrag(int dx, int dy);
	void applyEye();

	void begin(const ofRectangle & viewport);
	void end();

	/// Camera used for matrices / eye position.
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
