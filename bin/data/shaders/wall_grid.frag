#version 150
// Grayscale level only — no displacement / parallax / height ray-march.

uniform float uSpatialMode;
uniform float uEncodingMode;
uniform float uMeterLitCols;
uniform float uRmsMapMax;
uniform float uLitEnd[16];
uniform float uRms[16];

in vec2 vTex;
flat in int vChannel;

out vec4 fragColor;

void main(){
	int row = clamp(int(floor(vTex.y * 16.0 - 1e-4)), 0, 15);
	int ch = row;

	float lum = 0.0;
	if(uEncodingMode < 0.5){
		float r = uRms[ch];
		lum = clamp(r / max(uRmsMapMax, 1e-6), 0.0, 1.0);
	}else{
		lum = clamp(uLitEnd[ch] / max(uMeterLitCols, 1.0), 0.0, 1.0);
	}

	if(uSpatialMode > 0.5){
		float stripe = 0.86 + 0.14 * sin(vTex.x * 64.0 * 6.2831853);
		lum *= stripe;
	}

	float grid = 0.0;
	float gu = fract(vTex.x * 64.0);
	float gv = fract(vTex.y * 16.0);
	if(gu > 0.94 || gv > 0.94){
		grid = 0.12;
	}
	lum = clamp(lum * (1.0 - grid), 0.0, 1.0);

	float shade = 0.08 + 0.92 * lum;
	fragColor = vec4(vec3(shade), 1.0);
}
