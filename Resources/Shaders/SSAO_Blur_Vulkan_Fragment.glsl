#version 450

layout (binding = 0) uniform sampler2D samplerSSAO;

layout (location = 0) in vec2 inUV;
layout (location = 0) out float outFragColor;

layout (constant_id = 0) const int blurRange = 2;

void main() 
{
	vec2 flippedUV = vec2(inUV.s, 1.0 - inUV.t);
	int n = 0;
	vec2 texelSize = 1.0 / vec2(textureSize(samplerSSAO, 0));
	float result = 0.0;
	for (int x = -blurRange; x < blurRange; x++) 
	{
		for (int y = -blurRange; y < blurRange; y++) 
		{
			vec2 offset = vec2(float(y), float(x)) * texelSize;
			result += texture(samplerSSAO, flippedUV + offset).r;
			n++;
		}
	}
	
	outFragColor = result / (float(n));
}