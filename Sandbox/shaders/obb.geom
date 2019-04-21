#version 440 core

layout (points) in;
layout (line_strip, max_vertices = 34) out;

uniform vec3 uMin;
uniform vec3 uMax;

uniform mat4 uModelViewProjectionMatrix;

void main() 
{
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMin.y, uMin.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMin.y, uMin.z, 0.0)); EmitVertex();
	EndPrimitive();

	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMax.y, uMin.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMax.y, uMin.z, 0.0)); EmitVertex();
	EndPrimitive();	
	
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMin.y, uMax.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMin.y, uMax.z, 0.0)); EmitVertex();
	EndPrimitive();

	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMax.y, uMax.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMax.y, uMax.z, 0.0)); EmitVertex();
	EndPrimitive();

	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMin.y, uMin.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMin.y, uMax.z, 0.0)); EmitVertex();
	EndPrimitive();

	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMax.y, uMin.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMax.y, uMax.z, 0.0)); EmitVertex();
	EndPrimitive();	
	
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMin.y, uMin.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMin.y, uMax.z, 0.0)); EmitVertex();
	EndPrimitive();

	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMax.y, uMin.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMax.y, uMax.z, 0.0)); EmitVertex();
	EndPrimitive();

	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMin.y, uMin.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMax.y, uMin.z, 0.0)); EmitVertex();
	EndPrimitive();

	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMin.y, uMin.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMax.y, uMin.z, 0.0)); EmitVertex();
	EndPrimitive();	
	
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMin.y, uMax.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMin.x, uMax.y, uMax.z, 0.0)); EmitVertex();
	EndPrimitive();

	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMin.y, uMax.z, 0.0)); EmitVertex();
	gl_Position = uModelViewProjectionMatrix * (gl_in[0].gl_Position + vec4(uMax.x, uMax.y, uMax.z, 0.0)); EmitVertex();
	EndPrimitive();
}