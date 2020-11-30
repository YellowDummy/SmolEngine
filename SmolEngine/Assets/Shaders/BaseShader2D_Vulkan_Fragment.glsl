#version 450 core

layout(location = 0) out vec4 color;

// Light2D

struct Light2DBuffer
{
	vec4 LightColor;
	vec4 Position;
	float Radius;
	float Intensity;
};

// Scene Environment Uniforms

layout(std140, binding = 1) uniform LightBuffer
{
	Light2DBuffer[100] LightData;
};

layout(location = 100) uniform int Ligh2DCount;

layout(location = 0) uniform sampler2D u_Textures[32]; // note: no need to put textures inside uniform buffer

// Batch Buffer

struct BatchData
{
	vec4 position;
	vec4 color;
	vec4 uv;
	float ambientValue;
	float textureID;
	int textMode;
};

layout (location = 22) in flat BatchData v_Data; // note: must have "flat" flag

void main()
{
	vec4 ambient = vec4(v_Data.color * v_Data.ambientValue);
	vec4 result = texture(u_Textures[int(v_Data.textureID)], v_Data.uv.xy);

	color = result;

	if(v_Data.textMode == 1)
	{
		result = vec4(v_Data.color.rgb, texture(u_Textures[int(v_Data.textureID)], v_Data.uv.xy).r);
	}
	else
	{
		result.rgb = (result.rgb * ambient.rgb);

		vec3 tempColor = vec3(0.0, 0.0, 0.0);
		

		for(int i = 0; i < Ligh2DCount; ++i)
		{
			float intensity = ( LightData[i].Radius / length(v_Data.position.xy - LightData[i].Position.xy) ) * LightData[i].Intensity;
			intensity = atan(intensity, sqrt(length(v_Data.position.xy - LightData[i].Position.xy)));

			tempColor.rgb += (LightData[i].LightColor.rgb * intensity);
			
			for(int x = 0; x < Ligh2DCount; ++x)
			{
				if(x == i)
				{
					continue;
				}

				float intensity_new = ( LightData[x].Radius / length(v_Data.position.xy - LightData[x].Position.xy) ) * LightData[x].Intensity;
				intensity_new = atan(intensity_new, sqrt(length(v_Data.position.xy - LightData[x].Position.xy)));

				tempColor.rgb += (LightData[x].LightColor.rgb * (intensity_new) - (LightData[i].LightColor.rgb * intensity));
			}
		}
		
		result.rgb *= tempColor;
	}

	// color = result;
}