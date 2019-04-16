#version 440 core

layout (location = 0) in vec3 Position;
layout (location = 2) in vec3 Normal;

out Vertex {
	vec3 normal;
} vertex;

void main() 
{
	gl_Position = vec4(Position, 1.0);
	vertex.normal = Normal;
}