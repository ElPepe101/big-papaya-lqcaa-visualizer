#version 150
// VS-only: Y displacement from blended 16ch field + bread displacement.png (no FS parallax for audio).

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;
uniform float uDispAmp;
uniform float uDispTexScale;
uniform float uMeterLitCols;
uniform float uLitEnd[16];
uniform sampler2D uDisplacement;

in vec4 position;
in vec3 normal;
in vec2 texcoord;

out vec3 vWorldPos;
out vec3 vT;
out vec3 vB;
out vec3 vNgeom;
out vec2 vTex;

// Gaussian weights on a soft 4×4 layout; normalized so multiple channels affect each UV.
void blendedActivity(out float activity, out float weightSum){
	weightSum = 0.0;
	float acc = 0.0;
	const float sigma = 0.38;
	for(int c = 0; c < 16; c++){
		float ci = float(c % 4);
		float cj = float(c / 4);
		vec2 ctr = vec2((ci + 0.5) / 4.0, (cj + 0.5) / 4.0);
		vec2 d = (texcoord.xy - ctr) * 4.0;
		float ww = exp(-dot(d, d) / (2.0 * sigma * sigma));
		weightSum += ww;
		acc += ww * uLitEnd[c];
	}
	weightSum = max(weightSum, 1e-4);
	float blended = acc / weightSum;
	float meanL = 0.0;
	for(int c = 0; c < 16; c++){
		meanL += uLitEnd[c];
	}
	meanL /= 16.0;
	activity = clamp((blended * 0.82 + meanL * 0.18) / uMeterLitCols, 0.0, 1.0);
}

void main(){
	float act;
	float wsum;
	blendedActivity(act, wsum);

	float dispMag = uDispAmp * pow(act, 0.65);

	vec3 nModel = normalize(normal.xyz);
	float h = texture(uDisplacement, texcoord).r;
	float bump = (h - 0.5) * 2.0 * uDispTexScale;

	vec3 displaced = position.xyz + nModel * (dispMag + bump);

	vec4 world = modelMatrix * vec4(displaced, 1.0);
	vWorldPos = world.xyz;
	vNgeom = normalize(mat3(modelMatrix) * nModel);

	// Tangent frame for XZ plane (+Y up): T = +X, B = +Z (matches UV u→x, v→z).
	vT = normalize(mat3(modelMatrix) * vec3(1.0, 0.0, 0.0));
	vB = normalize(mat3(modelMatrix) * vec3(0.0, 0.0, 1.0));

	vTex = texcoord;
	gl_Position = modelViewProjectionMatrix * vec4(displaced, 1.0);
}
