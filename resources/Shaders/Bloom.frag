#version 450

layout (binding = 0) uniform sampler2D colorSampler;

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;

layout(std140, binding = 34) uniform BloomProperties
{   
	float exposure;
	float threshold;
	float scale;
	float strength;

} bloomState;

// From the OpenGL Super bible
	const float weights[] = float[](0.0024499299678342,
									0.0043538453346397,
									0.0073599963704157,
									0.0118349786570722,
									0.0181026699707781,
									0.0263392293891488,
									0.0364543006660986,
									0.0479932050577658,
									0.0601029809166942,
									0.0715974486241365,
									0.0811305381519717,
									0.0874493212267511,
									0.0896631113333857,
									0.0874493212267511,
									0.0811305381519717,
									0.0715974486241365,
									0.0601029809166942,
									0.0479932050577658,
									0.0364543006660986,
									0.0263392293891488,
									0.0181026699707781,
									0.0118349786570722,
									0.0073599963704157,
									0.0043538453346397,
									0.0024499299678342);

void main(void)
{
	vec3 texColor =  texture(colorSampler, inUV).rgb;
	vec2 tex_offset = 1.0 / textureSize(colorSampler, 0); // gets size of single texel
    vec3 result = texColor * weights[0]; // current fragment's contribution
	for (int i = 0; i < weights.length(); i++)
	{
		result += (texture(colorSampler, inUV + vec2(tex_offset.x * i, 0.0)).rgb * weights[i]);
        result += (texture(colorSampler, inUV - vec2(tex_offset.x * i, 0.0)).rgb * weights[i]);
	}

	outColor = vec4(result, 1.0);
}