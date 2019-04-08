#version 440 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TextureUV;

out vec2 ioTextureUV;

void main() 
{
	ioTextureUV = TextureUV;

	gl_Position = vec4(Position, 1.0);
}