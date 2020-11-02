#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform float u_AmbientValue;
uniform mat4 u_Transform;

out OutData
{
	vec4 position;
	vec2 uv;
	float ambientValue;

} v_Data;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);

	v_Data.position = u_Transform * vec4(a_Position, 1.0);
	v_Data.uv = a_TexCoord;
	v_Data.ambientValue = u_AmbientValue;
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

uniform Light2DBuffer LightData[250];

uniform int u_Ligh2DBufferSize;

in OutData
{
	vec4 position;
	vec2 uv;
	float ambientValue;

} v_Data;

uniform vec4 u_Color;

uniform float u_TilingFactor;

uniform sampler2D u_Texture;
uniform bool u_TextMode;

void main()
{
	vec4 ambient = (u_Color * v_Data.ambientValue);
	vec4 result = texture(u_Texture, v_Data.uv * u_TilingFactor);

	if(u_TextMode)
	{
		result = vec4(ambient.rgb, texture(u_Texture, v_Data.uv * u_TilingFactor).r);
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