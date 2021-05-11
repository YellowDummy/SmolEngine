#version 450 core

layout (location = 0) in vec2 uv;
layout (location = 0) out vec4 color;

layout (binding = 5) uniform sampler2D colorSampler;
layout (binding = 6) uniform sampler2D positionSampler;
layout (binding = 7) uniform sampler2D normalsSampler;

struct PointLight
{
	vec4 position;
	vec4 color;
	vec4 params; // x - intensity 
};

layout(std140, binding = 2) readonly buffer PointLightBuffer
{   
	PointLight lights[];
};

void main()
{
	vec4 albedro = texture(colorSampler, uv);
	vec4 position = texture(positionSampler, uv);
	vec4 normals = texture(normalsSampler, uv);

	color = albedro;
}