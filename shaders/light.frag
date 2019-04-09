#version 440 core

out vec4 FragmentColor;

struct Light {
	vec3 Position;
	vec3 Color;
	float Diffuse;
	float Specular;
	float Linear;
	float Quadratic;
	float Radius;
	bool Shadow;
};

struct LightVector {
	vec3 Direction;
	vec3 Color;
	vec3 Intensity;
	bool Enabled;
};

struct LightAmbient {
	vec3 Color;
	vec3 Intensity;
	bool Enabled;
};

uniform samplerCubeArray uPointShadowMap;

uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uAlbedoSpecular;
uniform sampler2D uShadowVector;

uniform Light uLight[64];
uniform int uLightCount;

uniform LightVector uVectorLight;
uniform LightAmbient uAmbientLight;

uniform vec3 uCameraPosition;

uniform int uDrawMode = 0;

in vec2 ioTextureUV;

vec3 samples[20] = vec3[] 
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float getFragmentShadow(vec3 fragpos, vec3 lightpos, int sid) 
{
	vec3 distance = fragpos - lightpos;
	float current = length(distance);
	float shadow = 0.0;
	float bias = 0.5;
	float dist = length(uCameraPosition - lightpos);
	float disk = (1.0 + (dist / 100.0)) / 25.0;
	
	for (int i = 0; i < 20; i++) 
	{
		if (current - bias > texture(uPointShadowMap, vec4(distance + samples[i] * disk, sid)).r * 100.0) 
		{
			shadow += 1.0;
		}
	}

	return shadow / 20.0;
}

vec3 getVectorLight(vec3 diff, float spec, vec3 norm, vec3 viewdir, vec3 fragpos) 
{
	vec3 lightdir = normalize(-uVectorLight.Direction);
	float diffuse = max(dot(norm, lightdir), 0.0);
	vec3 halfdir = normalize(lightdir + viewdir);
	float specular = pow(max(dot(norm, halfdir), 0.0), 32.0);
	return (diffuse * diff * uVectorLight.Intensity.y * uVectorLight.Color + specular * spec * uVectorLight.Intensity.z * uVectorLight.Color);
}

void main() 
{
	vec3 fragpos = texture(uPosition, ioTextureUV).rgb;
	vec3 normal = texture(uNormal, ioTextureUV).rgb;
	vec3 diffuse = texture(uAlbedoSpecular, ioTextureUV).rgb;
	float specular = texture(uAlbedoSpecular, ioTextureUV).a;

	vec3 color = vec3(0.0);

	if (uDrawMode == 0) 
	{
		vec3 viewdir = normalize(uCameraPosition - fragpos);

		int sid = 0;
		for (int i = 0; i < min(uLightCount, 64); ++i) 
		{
			float distance = length(uLight[i].Position - fragpos);
			if (distance < uLight[i].Radius) 
			{
				vec3 lightdir = normalize(uLight[i].Position - fragpos);
				float diff = max(dot(lightdir, normal), 0.0);
				vec3 halfdir = normalize(lightdir + viewdir);
				float spec = pow(max(dot(normal, halfdir), 0.0), 32.0);
				float atte = 1.0 / (1.0 + uLight[i].Linear * distance + uLight[i].Quadratic * distance * distance);
				float inte = uLight[i].Shadow ? 1.0 - getFragmentShadow(fragpos, uLight[i].Position, sid++) : 1.0;
				vec3 diffcol = (uLight[i].Color * uLight[i].Diffuse * diff) * atte;
				vec3 speccol = (uLight[i].Color * uLight[i].Specular * spec) * atte;
				color += (diffuse * diffcol + specular * speccol) * inte;
			}
		}

		if (uVectorLight.Enabled) 
		{
			color += getVectorLight(diffuse, specular, normal, viewdir, fragpos);
		}

		if (uAmbientLight.Enabled) 
		{
			color += diffuse * uAmbientLight.Color * uAmbientLight.Intensity.x;
		}
	} 
	else if (uDrawMode == 1) 
	{
		color = fragpos;
	}
	else if (uDrawMode == 2) 
	{
		color = normal;
	}
	else if (uDrawMode == 3) 
	{
		color = diffuse;
	}
	else if (uDrawMode == 4) 
	{
		color = vec3(specular);
	}
	else 
	{
		color = vec3(1.0);
	}

	FragmentColor = vec4(color, 1.0);
}