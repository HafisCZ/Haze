#version 410 core

layout (location = 0) out vec4 Color;

in vec2 ioTextureUV;
in vec3 ioWorldPosition;
in vec3 ioNormal;

uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;
uniform sampler2D uMetallicMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uAoMap;

uniform vec3 uLightPositions[5];
uniform vec3 uLightColors[5];

uniform vec3 uCameraPosition;

const float PI = 3.14159265359;

vec3 GetNormalFromMap() 
{
	vec3 tangentNormal = texture(uNormalMap, ioTextureUV).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(ioWorldPosition);
	vec3 Q2 = dFdy(ioWorldPosition);
	vec2 st1 = dFdx(ioTextureUV);
	vec2 st2 = dFdy(ioTextureUV);

	vec3 N = normalize(ioNormal);
	vec3 T = normalize(Q1 * st1.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));

	return normalize(mat3(T, B, N) * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeomtrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeomtrySmith(vec3 N, vec3 V, vec3 L, float roughness) 
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);

	float ggx2 = GeomtrySchlickGGX(NdotV, roughness);
	float ggx1 = GeomtrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() 
{
	// TODO


}