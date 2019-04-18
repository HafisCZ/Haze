#version 440 core

layout (location = 0) out vec3 Position;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec4 AlbedoSpecular;

uniform bool uTexture[3];
uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;

in vec2 ioTextureUV;
in vec3 ioFragmentPosition;
in vec3 ioNormal;
in mat3 ioTangentMatrix;

void main() 
{
	Position = ioFragmentPosition;

	if (uTexture[1]) 
	{
		Normal = normalize(ioTangentMatrix * normalize(texture(uTexture1, ioTextureUV).rgb * 2.0 - 1.0));
	} 
	else 
	{
		Normal = normalize(ioNormal);
	}

	AlbedoSpecular.rgb = uTexture[0] ? texture(uTexture0, ioTextureUV).rgb : vec3(1.0);
	AlbedoSpecular.a = uTexture[2] ? texture(uTexture2, ioTextureUV).r : 0.5;
}
