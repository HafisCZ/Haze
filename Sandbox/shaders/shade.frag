#version 440 core

uniform vec3 uPosition[5];

in vec4 ioFragmentPosition;

void main() 
{
	gl_FragDepth = length(ioFragmentPosition.xyz - uPosition[gl_Layer / 6]) / 100.0;
}