#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices=90) out;

uniform mat4 uFace[30];
uniform int uFaceCount;

out vec4 ioFragmentPosition;

void main() 
{
	for (int face = 0; face < min(uFaceCount, 30); face++) 
	{
		gl_Layer = face;

		for (int i = 0; i < 3; i++) 
		{
			ioFragmentPosition = gl_in[i].gl_Position;

			gl_Position = uFace[face] * ioFragmentPosition;

			EmitVertex();
		}

		EndPrimitive();
	}
}