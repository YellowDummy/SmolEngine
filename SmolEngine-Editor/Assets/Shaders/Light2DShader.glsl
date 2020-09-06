#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
//layout(location = 2) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_LightCood;
out vec3 v_Normal;

void main()
{
	v_LightCood = a_Position;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;
uniform float u_LightIntensity;

in vec3 v_LightCood;

void main()
{
	//float distance = length(v_LightCood);

	float t = u_LightIntensity - sqrt(v_LightCood.x * v_LightCood.x + v_LightCood.y * v_LightCood.y);

	//float attenuation = 1.0 / distance;
	//vec4 colorValue = vec4(attenuation, attenuation, attenuation, pow(attenuation, 3)) * vec4(u_Color);

	color = vec4(u_Color) * t;
}

