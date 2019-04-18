#version 410 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TextureUV;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

uniform mat4 uModelViewProjectionMatrix;

out vec2 ioTextureUV;

void main() 
{
	gl_Position = uModelViewProjectionMatrix * vec4(Position, 1.0);

	ioTextureUV = TextureUV;
}