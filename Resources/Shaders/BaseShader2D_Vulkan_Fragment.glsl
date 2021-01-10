#version 450 core

layout(location = 0) out vec4 color;

// Light2D

struct Light2DBuffer
{
	vec4 LightColor;
	vec4 Position;
	vec4 Attributes; // r = radius, g = intensity
};

// Scene Environment Uniforms

layout(std140, binding = 1) uniform LightBuffer
{
	Light2DBuffer[100] LightData;
};

layout(binding = 2) uniform sampler2D u_Textures[4096]; // note: no need to put textures inside uniform buffer

layout(push_constant) uniform LightEnvironment
{
	int LightSources;
};

// Batch Buffer

struct BatchData
{
	vec4 position;
	vec4 color;
	vec4 uv;
	float ambientValue;
	float textureID;
	float textMode;
};

layout (location = 22) in BatchData v_Data;

void main()
{
	vec4 ambient = vec4(v_Data.color * v_Data.ambientValue);
	vec4 result = texture(u_Textures[int(v_Data.textureID)], v_Data.uv.xy);

	if(v_Data.textMode == 1.0)
	{
		result = vec4(v_Data.color.rgb, texture(u_Textures[int(v_Data.textureID)], v_Data.uv.xy).r);
	}
	else
	{
		result.rgb = (result.rgb * ambient.rgb);

		vec3 tempColor = vec3(0.0, 0.0, 0.0);
		
		for(int i = 0; i < LightSources; ++i)
		{
			float intensity = ( LightData[i].Attributes.r / length(v_Data.position.xy - LightData[i].Position.xy) ) * LightData[i].Attributes.g;
			intensity = atan(intensity, sqrt(length(v_Data.position.xy - LightData[i].Position.xy)));

			tempColor.rgb += (LightData[i].LightColor.rgb * intensity);
			
			for(int x = 0; x < LightSources; ++x)
			{
				if(x == i)
				{
					continue;
				}

				float intensity_new = ( LightData[x].Attributes.r / length(v_Data.position.xy - LightData[x].Position.xy) ) * LightData[x].Attributes.g;
				intensity_new = atan(intensity_new, sqrt(length(v_Data.position.xy - LightData[x].Position.xy)));

				tempColor.rgb += (LightData[x].LightColor.rgb * (intensity_new) - (LightData[i].LightColor.rgb * intensity));
			}
		}
		
		result.rgb *= tempColor;
	}

	color = result;
}