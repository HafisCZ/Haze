#version 410 core

layout (location = 0) out vec4 Color;

in vec2 eTextureUV;

uniform vec4 uColor;

uniform sampler2D uTexture;
uniform bool uTextureEnable;

void main() 
{
	Color = uTextureEnable ? texture(uTexture, eTextureUV) * vec4(1.0f) : uColor;
}