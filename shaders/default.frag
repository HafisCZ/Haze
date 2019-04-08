#version 410 core

layout (location = 0) out vec4 Color;

uniform vec4 uColor;

uniform bool uTexture[3];
uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;

in vec2 ioTextureUV;

void main() 
{
	if (uTexture[0]) {
		Color = texture(uTexture0, ioTextureUV);
	} else {
		Color = uColor;
	}
}