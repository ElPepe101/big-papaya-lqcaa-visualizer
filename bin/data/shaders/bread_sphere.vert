#version 150
// Only this stage offsets vertices: audio-driven radial push + bread displacement texture (then MVP).

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;
uniform float uDispAmp;
uniform float uDispTexScale;
uniform float uMeterLitCols;
uniform float uLitEnd[16];
uniform sampler2D uDisplacement;

in vec4 position;
in vec4 color;
in vec3 normal;
in vec2 texcoord;

out vec3 vWorldPos;
out vec3 vT;
out vec3 vB;
out vec3 vNgeom;
out vec2 vTex;

void main(){
	int ch = clamp(int(floor(color.r + 0.5)), 0, 15);
	float colIdx = color.g * uMeterLitCols;
	float lit = uLitEnd[ch];
	// Fractional "on" amount for this azimuth column (same RMS→column mapping as before).
	float fill = clamp(lit - colIdx, 0.0, 1.0);
	// Soften: keeps motion readable without full 0/1 jumps.
	float dispMag = uDispAmp * pow(fill, 0.65);

	vec3 nModel = normalize(normal.xyz);
	// Mid-gray = no offset; brighter outward, darker inward (same UVs as albedo).
	float h = texture(uDisplacement, texcoord).r;
	float bump = (h - 0.5) * 2.0 * uDispTexScale;
	vec3 displaced = position.xyz + nModel * (dispMag + bump);

	vec4 world = modelMatrix * vec4(displaced, 1.0);
	vWorldPos = world.xyz;
	vNgeom = normalize(mat3(modelMatrix) * nModel);

	// Analytic tangent basis on sphere (phi = atan(z,x)) for stable normal mapping.
	vec3 P = vWorldPos;
	float phi = atan(P.z, P.x);
	float st = sqrt(max(0.0, 1.0 - vNgeom.y * vNgeom.y));
	vec3 dP_dphi = vec3(-st * sin(phi), 0.0, st * cos(phi));
	vT = normalize(dP_dphi - dot(dP_dphi, vNgeom) * vNgeom);
	vB = normalize(cross(vNgeom, vT));

	vTex = texcoord;
	gl_Position = modelViewProjectionMatrix * vec4(displaced, 1.0);
}
