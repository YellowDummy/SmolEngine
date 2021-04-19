#version 450 core

layout (location = 0) out vec4 outColor0;
layout (location = 1) out vec4 outColor1;


layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in float v_Gamma;
layout(location = 2) in float v_Exposure;

layout (binding = 1) uniform samplerCube samplerCubeMap;

// From http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 Uncharted2Tonemap(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

void main()
{
	vec3 color = texture(samplerCubeMap, v_WorldPos).rgb;

	// Tone mapping
	color = Uncharted2Tonemap(color * v_Exposure);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / v_Gamma));

	// Color with manual exposure into attachment 0
	outColor0.rgb = vec3(1.0) - exp(-color.rgb * 0.8);

	// Bright parts for bloom into attachment 1
	float l = dot(outColor0.rgb, vec3(0.2126, 0.7152, 0.0722));
	float threshold = 0.75;
	outColor1.rgb = (l > threshold) ? outColor0.rgb : vec3(0.0);
	outColor1.a = 1.0;
}