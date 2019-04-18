#version 440 core

layout (location = 0) out vec4 Color;

uniform samplerCube uTextureCube;

in vec3 ioTextureUV;

void main() 
{
	Color = texture(uTextureCube, ioTextureUV);
}