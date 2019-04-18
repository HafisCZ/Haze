#version 440 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Placeholder2;
layout (location = 2) in vec2 TextureUV;
layout (location = 3) in vec3 Placeholder3;
layout (location = 4) in vec3 Placeholder4;

out vec2 ioTextureUV;

void main() 
{
	ioTextureUV = TextureUV;

	gl_Position = vec4(Position, 1.0);
}