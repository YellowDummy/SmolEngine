#version 460

layout (location = 0) in vec2 inUV;

layout (binding = 0) uniform sampler2D colorSampler;
layout (binding = 1) uniform sampler2D bloomSampler;
layout (binding = 2) uniform sampler2D DirtSampler;

layout (location = 0) out vec4 outFragColor;

layout(push_constant) uniform ConstantData
{
    uint ppState;
    uint is_vertical_blur; 
    uint is_dirt_mask;
    float mask_intensity;
    float mask_normal_intensity;
};

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

void main() 
{
    vec4 color = texture(colorSampler, inUV);
    switch(ppState)
    {
        case 1:
        {
            vec4 bloom = vec4(0.0);
            vec4 dirtColor = vec4(0.0);
            if(is_vertical_blur == 1)
            {
	            vec2 tex_offset = 1.0 / textureSize(bloomSampler, 0); 
                vec3 result = texture(bloomSampler, inUV).rgb * weights[0] * bloomState.scale;
                for (int i = 0; i < weights.length(); i++)
	            {
		          result += texture(bloomSampler, inUV + vec2(0.0, tex_offset.y * i)).rgb * weights[i] * bloomState.strength;
                  result += texture(bloomSampler, inUV - vec2(0.0, tex_offset.y * i)).rgb * weights[i] * bloomState.strength;
                }

                bloom = vec4(result, 1.0);
            }
            else
            {
                bloom = texture(bloomSampler, inUV);
            }

            if(is_dirt_mask == 1)
            {
                if(bloom.r > 0.2 && bloom.g > 0.2 && bloom.b > 0.2)
                {
                    dirtColor = texture(DirtSampler, inUV) * bloom * mask_intensity;
                }
                else
                {
                    dirtColor = texture(DirtSampler, inUV) * mask_normal_intensity;
                }
            }
            
            bloom += dirtColor;
            color = vec4(color.rgb + bloom.rgb, 1.0);
            break;
        }
    }

    outFragColor = color;
}