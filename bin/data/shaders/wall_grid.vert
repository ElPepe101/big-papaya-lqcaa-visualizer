#version 150
// VS-only motion along wall normal (+Z); no FS height/parallax for metering.

uniform mat4 modelViewProjectionMatrix;
uniform float uTime;
uniform float uNoiseAmp;
uniform float uPhase[16];

in vec4 position;
in vec3 normal;
in vec2 texcoord;
in vec4 color;

out vec2 vTex;

float wave(vec2 uv, float ph){
	vec2 p = uv * vec2(6.5, 5.0) + vec2(ph * 1.7, ph * 0.9);
	float a = sin(p.x + uTime * 0.85) * cos(p.y - uTime * 0.62);
	float b = sin(p.y * 2.1 + uTime * 1.1 + ph);
	return (a + b) * 0.5;
}

void main(){
	int ch = clamp(int(floor(color.r * 15.0 + 0.5)), 0, 15);
	float ph = uPhase[ch];
	vec3 n = normalize(normal);
	float disp = uNoiseAmp * wave(texcoord, ph);
	vec3 wp = position.xyz + n * disp;
	vTex = texcoord;
	gl_Position = modelViewProjectionMatrix * vec4(wp, 1.0);
}
