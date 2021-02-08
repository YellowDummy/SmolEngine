#version 450 core

layout(location = 0) out vec4 o_color;

layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in float v_Gamma;
layout(location = 2) in float v_Exposure;

layout (binding = 15) uniform samplerCube samplerCubeMap;

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
	
	o_color = vec4(color, 1.0);
}