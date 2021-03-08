#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TextMode;
layout(location = 4) in float a_TexIndex;

// Batch Buffer

struct BatchData
{
	vec4 position;
	vec4 color;
	vec4 uv;
	float ambientValue;
	float textureID;
	float textMode;
	float lightSources;
};


layout(location = 22) out BatchData v_Data;

layout(push_constant) uniform LightEnvironment
{
	mat4 u_ViewProjection;
	float u_LightSources;
};

void main()
{
	v_Data.position = vec4(a_Position, 1.0);
	v_Data.color = a_Color;
	v_Data.uv = vec4(a_TexCoord, 1, 1);
	v_Data.ambientValue = 1.0;
	v_Data.textureID = a_TexIndex;
	v_Data.textMode = a_TextMode;
	v_Data.lightSources = u_LightSources;

	gl_Position = u_ViewProjection * vec4(a_Position, 1);
}