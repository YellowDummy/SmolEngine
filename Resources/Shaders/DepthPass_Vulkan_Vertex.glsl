#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in vec4 a_Color;

#define SHADOW_MAP_CASCADE_COUNT 4

layout(push_constant) uniform PushConsts {
	mat4 model;
	int cascadeIndex;
} pushConsts;

layout (binding = 1) uniform UBO {
	mat4[SHADOW_MAP_CASCADE_COUNT] cascadeViewProjMat;
} ubo;

layout (location = 0) out vec2 outUV;

out gl_PerVertex {
	vec4 gl_Position;   
};

void main()
{
	outUV = a_UV;
	vec3 pos = vec4(pushConsts.model * vec4(a_Position, 1)).xyz;
	gl_Position =  ubo.cascadeViewProjMat[pushConsts.cascadeIndex] * vec4(pos, 1.0);
}