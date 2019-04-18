#version 410 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TextureUV;

uniform mat4 uViewProjectionMatrix;
uniform mat4 uModelMatrix;

out vec2 ioTextureUV;
out vec3 ioWorldPosition;
out vec3 ioNormal;

void main() 
{
	ioTextureUV = TextureUV;
	ioNormal = mat3(uModelMatrix) * Normal;
	ioWorldPosition = vec3(uModelMatrix * vec4(Position, 1.0));

	gl_Position = uViewProjectionMatrix * vec4(ioWorldPosition, 1.0);
}