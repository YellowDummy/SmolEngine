#version 460 core

layout (location = 0) in vec2 inUV;

layout (binding = 0) uniform sampler2D colorSampler;
layout (binding = 1) uniform sampler2D bloomSampler;
layout (binding = 2) uniform sampler2D materialsMap;
layout (binding = 3) uniform sampler2D occlusionMap;
layout (binding = 4) uniform sampler2D DirtSampler;

layout (location = 0) out vec4 outFragColor;

layout(std140, binding = 34) uniform BloomProperties
{   
	float exposure;
	float threshold;
	float scale;
	float strength;
} bloomState;

layout(push_constant) uniform ConstantData
{
    uint ppState;
    uint is_dirt_mask;
    float mask_intensity;
    float mask_normal_intensity;
};

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


vec3 applyBloom(vec4 color)
{
    float emission = texture(materialsMap, inUV).w;
    float strength = emission * bloomState.strength;
    if(color.a == 0.0) // background
    {
        strength = 1.0;
    }

	vec2 tex_offset = 1.0 / textureSize(bloomSampler, 0); 
    vec3 result = texture(bloomSampler, inUV).rgb * weights[0];
    for (int i = 0; i < weights.length(); i++)
	{
	     result += texture(bloomSampler, inUV + vec2(0.0, tex_offset.y * i)).rgb * weights[i] * strength;
        result += texture(bloomSampler, inUV - vec2(0.0, tex_offset.y * i)).rgb * weights[i] * strength;
    }

    return vec3(result * bloomState.scale);
}

vec4 applyVolumetricLight(vec3 screen)
{
    vec2 lightPositionOnScreen = vec2(300, 300);
    vec4 color = vec4(0);
    const float exposure = 1.0;
    const float decay = 1.0;
    const float density = 1.0;
    const float weight = 0.01;
    const int NUM_SAMPLES = 100;

    vec2 texCoord = inUV;
    vec2 deltaTextCoord = vec2( texCoord - lightPositionOnScreen.xy );
    deltaTextCoord *= 1.0 / float(NUM_SAMPLES) * density;
    float illuminationDecay = 1.0;
    
    for(int i=0; i < 100; i++)
    {
		texCoord -= deltaTextCoord;
		vec3 samp = texture(colorSampler, texCoord).rgb;
		samp *= illuminationDecay * weight;
		color.rgb += samp;
		illuminationDecay *= decay;
    }

	color *= exposure;
    return color;
}

vec3 applyMask(vec3 bloom)
{
    vec4 maskColor = vec4(0.0);
    if(is_dirt_mask == 1)
    {
        if(bloom.r > 0.1 && bloom.g > 0.1 && bloom.b > 0.1)
        {
            maskColor = texture(DirtSampler, inUV) * vec4(bloom, 1) * mask_intensity;
        }
        else
        {
            maskColor = texture(DirtSampler, inUV) * mask_normal_intensity;
        }
    }

    return maskColor.rgb;
}

void main() 
{
    vec4 color = texture(colorSampler, inUV);
    switch(ppState)
    {
        case 1:
        {
            vec3 bloom = applyBloom(color);
            vec3 dirtColor = applyMask(bloom);
            
            bloom += dirtColor;
            color = vec4(color.rgb + bloom, 1.0);
            break;
        }
    }
    
    outFragColor = color;
}