#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in ivec4 a_BoneIDs;
layout(location = 5) in vec4 a_Weight;

layout(push_constant) uniform ConstantData
{
    mat4 viewProj;
};

layout (location = 0)  out vec3 v_FragPos;
layout (location = 1)  out vec3 v_Normal;
layout (location = 2)  out vec2 v_UV;
layout (location = 3)  out vec3 v_Tangent;

void main()
{
	v_FragPos = a_Position;
	v_Normal =  a_Normal;
	v_UV = a_UV;
	v_Tangent = a_Tangent;

	gl_Position =  viewProj * vec4(a_Position, 1.0);
}