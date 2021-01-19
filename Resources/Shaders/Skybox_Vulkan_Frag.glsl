#version 450 core

layout(location = 0) out vec4 o_color;
layout(location = 1) in vec3 v_WorldPos;

layout (binding = 15) uniform samplerCube samplerCubeMap;

void main()
{
	o_color = texture(samplerCubeMap, v_WorldPos);
}