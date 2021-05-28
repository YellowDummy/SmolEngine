#version 450

layout (binding = 0) uniform sampler2D HDRSampler;

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

layout(push_constant) uniform ConstantData
{
    uint dir;
    float blurScale;
    float blurStrength;
};

void main() 
{
	float weight[5];
	weight[0] = 0.227027;
	weight[1] = 0.1945946;
	weight[2] = 0.1216216;
	weight[3] = 0.054054;
	weight[4] = 0.016216;

	vec3 result = vec3(0);
    vec2 tex_offset = 1.0 / textureSize(HDRSampler, 0) * blurScale; // gets size of single texel
	result = texture(HDRSampler, inUV).rgb * weight[0]; // current fragment's contribution
	for(int i = 1; i < 5; ++i)
	{
	if (dir == 1)
	{
		// H
		result += texture(HDRSampler, inUV + vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * blurStrength;
		result += texture(HDRSampler, inUV - vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * blurStrength;
	}
	else
	{
		// V
		result += texture(HDRSampler, inUV + vec2(0.0, tex_offset.y * i)).rgb * weight[i] * blurStrength;
		result += texture(HDRSampler, inUV - vec2(0.0, tex_offset.y * i)).rgb * weight[i] * blurStrength;
	}
	}
	outFragColor = vec4(result, 1.0);
}