#version 150
// PBR + distance fog (exponential, squared distance). Geometry displacement stays in bread_plane.vert.
// uDisplacement is not sampled here for audio/geometry offsets.

uniform vec3 uCameraPos;
uniform vec3 uLKey;
uniform vec3 uLFill;
uniform vec3 uLAccent;
uniform float uUseEnvDiffuse;
uniform float uUseEnvSpec;
uniform float uEnvMaxMips;
uniform float uMeterLitCols;
uniform float uLitEnd[16];
uniform float uHueMix;
uniform float uFogDensity;
uniform vec3 uFogColor;

uniform sampler2D uAlbedo;
uniform sampler2D uNormal;
uniform sampler2D uRoughness;
uniform sampler2D uAo;
uniform samplerCube uIrradiance;
uniform samplerCube uPrefilter;

in vec3 vWorldPos;
in vec3 vT;
in vec3 vB;
in vec3 vNgeom;
in vec2 vTex;

out vec4 fragColor;

const float PI = 3.14159265;
const vec3 KEY_RAD = vec3(2.25, 1.92, 1.58);
const vec3 FILL_RAD = vec3(0.38, 0.58, 0.95);
const vec3 ACC_RAD = vec3(0.95, 0.48, 1.15);
const vec3 RIM_RAD = vec3(0.55, 0.82, 1.08);

vec3 channelHue(int c){
	float h = float(c) / 16.0;
	float s = 0.55;
	float v = 0.92;
	float i = floor(h * 6.0);
	float f = h * 6.0 - i;
	float p = v * (1.0 - s);
	float q = v * (1.0 - s * f);
	float t = v * (1.0 - s * (1.0 - f));
	if(i < 1.0) return vec3(v, t, p);
	if(i < 2.0) return vec3(q, v, p);
	if(i < 3.0) return vec3(p, v, t);
	if(i < 4.0) return vec3(p, q, v);
	if(i < 5.0) return vec3(t, p, v);
	return vec3(v, p, q);
}

vec3 fakeDiffuse(vec3 R){
	vec3 aR = abs(R);
	float cc = max(0.0, abs(dot(aR, normalize(vec3(0.15, 0.85, 0.25)))));
	return vec3(clamp(cc * 0.35 + 0.45, 0.0, 1.0));
}

vec3 fakeSpecular(vec3 R, float rough){
	vec3 aR = abs(R);
	float m = rough * rough;
	float s = (1.0 - m * 0.95);
	s *= s;
	s *= s;
	float cc = max(0.0, abs(dot(aR, normalize(vec3(0.2, 0.6, 0.35)))));
	cc = pow(cc + 0.02, s * 90.0 + 2.0);
	return vec3(clamp(cc, 0.0, 1.0));
}

void main(){
	mat3 TBN = mat3(normalize(vT), normalize(vB), normalize(vNgeom));
	vec3 nm = texture(uNormal, vTex).xyz * 2.0 - 1.0;
	nm.y = -nm.y;
	vec3 N = normalize(TBN * nm);

	vec3 V = normalize(uCameraPos - vWorldPos);
	float roughTex = texture(uRoughness, vTex).r;
	float rough = clamp(roughTex * 0.92 + 0.06, 0.04, 1.0);
	float sumL = 0.0;
	for(int c = 0; c < 16; c++){
		sumL += uLitEnd[c];
	}
	float glo = clamp(sumL / (16.0 * max(uMeterLitCols, 1.0)), 0.0, 1.0);
	rough = clamp(rough + (1.0 - glo) * 0.22, 0.04, 1.0);

	vec3 albedo = texture(uAlbedo, vTex).rgb;
	float aoTex = clamp(texture(uAo, vTex).r, 0.04, 1.0);
	float aoDiff = aoTex;
	float aoSpec = mix(1.0, aoTex, 0.5);
	float aoDirect = aoTex;

	// --- Weighted channel hue (same Gaussian kernel as VS) for readable channel identity ---
	float wsum = 0.0;
	vec3 hueAcc = vec3(0.0);
	const float sigma = 0.38;
	for(int c = 0; c < 16; c++){
		float ci = float(c % 4);
		float cj = float(c / 4);
		vec2 ctr = vec2((ci + 0.5) / 4.0, (cj + 0.5) / 4.0);
		vec2 d = (vTex - ctr) * 4.0;
		float ww = exp(-dot(d, d) / (2.0 * sigma * sigma));
		wsum += ww;
		float lvl = clamp(uLitEnd[c] / max(uMeterLitCols, 1.0), 0.0, 1.0);
		hueAcc += ww * channelHue(c) * (0.35 + 0.65 * lvl);
	}
	wsum = max(wsum, 1e-4);
	vec3 hueMix = hueAcc / wsum;
	float act = 0.0;
	for(int c = 0; c < 16; c++){
		act += uLitEnd[c];
	}
	act = clamp(act / (16.0 * max(uMeterLitCols, 1.0)), 0.0, 1.0);
	albedo = mix(albedo, albedo * mix(vec3(1.0), hueMix, 0.55), uHueMix * (0.25 + 0.75 * act));

	vec3 R = reflect(-V, N);
	float NdotV = max(dot(N, V), 0.001);

	vec3 F0 = vec3(0.04);
	vec3 F = F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);
	vec3 kd = (vec3(1.0) - F) * albedo;

	vec3 diffuse;
	if(uUseEnvDiffuse > 0.5){
		vec3 irr = texture(uIrradiance, N).rgb;
		irr = irr / (irr + vec3(1.0));
		diffuse = irr * kd * (1.0 / PI) * aoDiff;
	}else{
		diffuse = fakeDiffuse(R) * kd * (1.0 / PI) * aoDiff;
	}

	vec3 spec;
	if(uUseEnvSpec > 0.5){
		float lod = rough * max(uEnvMaxMips - 1.0, 0.0);
		vec3 prefiltered = textureLod(uPrefilter, R, lod).rgb;
		prefiltered = prefiltered / (prefiltered + vec3(1.0));
		float specOcc = (1.0 - rough);
		specOcc *= specOcc;
		vec3 specular = prefiltered * (F * specOcc + vec3(0.04 * rough));
		float horizon = min(1.0 + dot(R, N), 1.0);
		specular *= horizon * horizon;
		spec = specular * aoSpec;
	}else{
		spec = fakeSpecular(R, rough) * F * aoSpec;
	}

	vec3 Lk = normalize(uLKey);
	vec3 Lf = normalize(uLFill);
	vec3 La = normalize(uLAccent);
	float Nk = max(dot(N, Lk), 0.0);
	float Nf = max(dot(N, Lf), 0.0);
	float Na = max(dot(N, La), 0.0);

	vec3 diffDirect = (KEY_RAD * Nk + FILL_RAD * Nf + ACC_RAD * Na) * albedo * (1.0 / PI) * aoDirect;

	vec3 Hk = normalize(Lk + V);
	float specPow = mix(8.0, 160.0, 1.0 - rough);
	float specK = pow(max(dot(N, Hk), 0.0), specPow) * (0.12 + 0.88 * (1.0 - rough));
	vec3 specDirect = KEY_RAD * F * specK * mix(1.0, aoDirect, 0.4);

	float rim = pow(1.0 - NdotV, 3.35);
	vec3 rimCol = RIM_RAD * rim * albedo * mix(0.35, 1.0, aoTex);

	vec3 lit = diffuse + spec + diffDirect + specDirect + rimCol;

	// Exponential fog: fog = 1 - exp(-density * dist^2 * 1e-6); see PlaneMeterView.cpp uFogDensity.
	float dist = length(uCameraPos - vWorldPos);
	float fogAmt = 1.0 - exp(-uFogDensity * dist * dist * 1e-6);
	fogAmt = clamp(fogAmt, 0.0, 1.0);
	lit = mix(lit, uFogColor, fogAmt);

	fragColor = vec4(lit, 1.0);
}
