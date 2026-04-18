#version 150
// PBR only here: all geometry displacement (bread height + audio) is applied in bread_sphere.vert — no FS parallax / height.

uniform vec3 uCameraPos;
uniform vec3 uLKey;
uniform vec3 uLFill;
uniform vec3 uLAccent;
uniform float uUseEnvDiffuse;
uniform float uUseEnvSpec;
uniform float uEnvMaxMips;

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
// Analytic lights (HDR-ish scalars on albedo); tuned for bread + IBL.
const vec3 KEY_RAD = vec3(2.25, 1.92, 1.58);
const vec3 FILL_RAD = vec3(0.38, 0.58, 0.95);
const vec3 ACC_RAD = vec3(0.95, 0.48, 1.15);
const vec3 RIM_RAD = vec3(0.55, 0.82, 1.08);

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

	vec3 albedo = texture(uAlbedo, vTex).rgb;
	float aoTex = clamp(texture(uAo, vTex).r, 0.04, 1.0);
	float aoDiff = aoTex;
	float aoSpec = mix(1.0, aoTex, 0.5);
	float aoDirect = aoTex;

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

	// --- Analytic lights (Lambert diffuse + tight Blinn spec from key) ---
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

	// Cool rim: grazing cyan lift (helps read silhouette vs HDRI).
	float rim = pow(1.0 - NdotV, 3.35);
	vec3 rimCol = RIM_RAD * rim * albedo * mix(0.35, 1.0, aoTex);

	fragColor = vec4(diffuse + spec + diffDirect + specDirect + rimCol, 1.0);
}
