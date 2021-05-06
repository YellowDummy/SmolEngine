#version 450 core

layout (location = 0) out vec4 outColor0;
layout (location = 1) out vec4 outColor1;


layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in float v_Exposure;

layout (binding = 1) uniform samplerCube samplerCubeMap;

void main()
{
	vec3 color = texture(samplerCubeMap, v_WorldPos).rgb;

	// Color with manual exposure into attachment 0
	outColor0.rgb = vec3(1.0) - exp(-color.rgb * 0.8);

	// Bright parts for bloom into attachment 1
	float l = dot(outColor0.rgb, vec3(0.2126, 0.7152, 0.0722));
	float threshold = 0.75;
	outColor1.rgb = (l > threshold) ? outColor0.rgb : vec3(0.0);
	outColor1.a = 1.0;
}