#version 410 core

layout (location = 0) out vec4 Color;

uniform vec4 uColor;

void main() 
{
	Color = uColor;
}