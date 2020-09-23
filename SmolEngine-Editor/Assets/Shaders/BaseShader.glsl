#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform float u_AmbientValue;
uniform mat4 u_Transform;

//animated sprites support
uniform vec2 u_TexCoord;

out vec2 v_TexCoord;
out float v_AmbientValue;

void main()
{
	v_AmbientValue = u_AmbientValue;
	if(u_TexCoord == vec2(-1.0, -1.0))
	{
	   v_TexCoord = a_TexCoord;
			
	}
	else
	{
	   v_TexCoord = u_TexCoord;
	}
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float v_AmbientValue;

uniform vec4 u_Color;
uniform float u_TilingFactor;
uniform sampler2D u_Texture;

void main()
{
	vec4 ambient = u_Color * v_AmbientValue;
	vec4 result = texture(u_Texture, v_TexCoord * u_TilingFactor);
	result.rgb = result.rgb * ambient.rgb;
	color = result;
}