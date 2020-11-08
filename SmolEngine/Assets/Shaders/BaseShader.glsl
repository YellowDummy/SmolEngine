#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

uniform mat4 u_ViewProjection;
uniform float u_AmbientValue;

// Batch

out BatchData
{
	vec4 position;

	vec4 color;

	vec2 uv;

	float ambientValue;

	float textureID;

} v_Data;

void main()
{

	v_Data.position = vec4(a_Position, 1.0);
	v_Data.color = a_Color;
	v_Data.uv = a_TexCoord;
	v_Data.ambientValue = u_AmbientValue;
	v_Data.textureID = a_TexIndex;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

// Light2D

struct Light2DBuffer
{
	vec3 LightColor;

	vec3 Position;

	float Radius;

	float Intensity;
};

uniform Light2DBuffer LightData[100];

uniform int u_Ligh2DBufferSize;

// Batch

in BatchData
{
	vec4 position;

	vec4 color;

	vec2 uv;

	float ambientValue;
	
	float textureID;

} v_Data;

uniform bool u_TextMode;

uniform sampler2D u_Textures[32];

void main()
{
	vec4 ambient = vec4(v_Data.color * v_Data.ambientValue);
	vec4 result = texture(u_Textures[int(v_Data.textureID)], v_Data.uv);

	if(u_TextMode)
	{
		result = vec4(v_Data.color.rgb, texture(u_Textures[int(v_Data.textureID)], v_Data.uv).r);
	}
	else
	{
		result.rgb = (result.rgb * ambient.rgb);

		vec3 tempColor = vec3(0.0, 0.0, 0.0);
		
		for(int i = 0; i < u_Ligh2DBufferSize; ++i)
		{
			float intensity = ( LightData[i].Radius / length(v_Data.position.xy - LightData[i].Position.xy) ) * LightData[i].Intensity;
			intensity = atan(intensity, sqrt(length(v_Data.position.xy - LightData[i].Position.xy)));

			tempColor.rgb += (LightData[i].LightColor * intensity);
			
			for(int x = 0; x < u_Ligh2DBufferSize; ++x)
			{
				if(x == i)
				{
					continue;
				}

				float intensity_new = ( LightData[x].Radius / length(v_Data.position.xy - LightData[x].Position.xy) ) * LightData[x].Intensity;
				intensity_new = atan(intensity_new, sqrt(length(v_Data.position.xy - LightData[x].Position.xy)));

				tempColor.rgb += (LightData[x].LightColor * (intensity_new) - (LightData[i].LightColor * intensity));
			}
		}
		
		result.rgb *= tempColor;
	}

	color = result;
}