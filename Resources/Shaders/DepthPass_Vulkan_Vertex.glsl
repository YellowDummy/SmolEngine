#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in vec4 a_Color;

struct ShaderData
{
	mat4 model;
	vec4 data;
};

layout(std140, binding = 25) readonly buffer ShaderDataBuffer
{   
	ShaderData instances[];
};

layout(push_constant) uniform PushConsts
 {
	mat4 depthMVP;
	uint dataOffset;
};

out gl_PerVertex 
{
    vec4 gl_Position;   
};

 
void main()
{
	mat4 model = instances[dataOffset + gl_InstanceIndex].model;
	vec4 pos = model * vec4(a_Position, 1.0);

	gl_Position =  depthMVP * pos;
}