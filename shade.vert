#version 440 core

layout (location = 0) in vec3 Position;

uniform mat4 uModelMatrix;

void main() 
{
	gl_Position = uModelMatrix * vec4(Position, 1.0);
}
