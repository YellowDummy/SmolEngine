#version 450 core

layout (location = 0) out vec4 color;

layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in float v_Exposure;

layout (binding = 1) uniform samplerCube samplerCubeMap;

void main()
{
	vec3 result = texture(samplerCubeMap, v_WorldPos).rgb;
	color = vec4(result, 0.0);
}