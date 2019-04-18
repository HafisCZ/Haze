#version 440 core

layout (location = 0) in vec3 Position;

uniform mat4 uTranslatedViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 ioTextureUV;

void main()
{
	ioTextureUV = Position;
	vec4 pos = uProjectionMatrix * uTranslatedViewMatrix * vec4(Position, 1.0);
	gl_Position = pos.xyww;
}