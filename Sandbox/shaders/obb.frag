#version 440 core

layout (location = 0) out vec4 Color;

uniform vec3 uColor = vec3(1.0, 1.0, 1.0);

void main() 
{
	Color = vec4(uColor, 1.0);
}