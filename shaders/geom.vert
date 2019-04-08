#version 440 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TextureUV;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

uniform mat4 uModelMatrix;
uniform mat4 uModelMatrixI;
uniform mat4 uViewProjectionMatrix;

out vec3 ioFragmentPosition;
out vec2 ioTextureUV;
out vec3 ioNormal;
out mat3 ioTangentMatrix;

void main() 
{
	vec4 worldPosition = uModelMatrix * vec4(Position, 1.0);

	ioFragmentPosition = worldPosition.xyz;
	ioTextureUV = TextureUV;
	ioNormal = mat3(uModelMatrixI) * Normal;

	mat3 normalMatrix = mat3(uModelMatrixI);
	vec3 t = normalize(normalMatrix * Tangent);
	vec3 n = normalize(normalMatrix * Normal);
	t = normalize(t - dot(t, n) * n);
	vec3 b = cross(t, n);

	ioTangentMatrix = mat3(uModelMatrix) * mat3(t, b, n);

	gl_Position = uViewProjectionMatrix * worldPosition;
}