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
layout (location = 3)  out vec4 v_Color;
layout (location = 4)  out vec4 v_WorldPos;
layout (location = 5)  out mat3 v_TBN;

void main()
{
	v_FragPos = vec3(vec4(a_Position, 1.0));
	v_Normal =  a_Normal;
	v_WorldPos = vec4(a_Position, 1.0);
	v_UV = a_UV;
	v_Color = vec4(1);

	// TBN matrix
	vec3 T = normalize(a_Tangent);
	vec3 N = normalize(a_Normal);
	vec3 B = normalize(cross(N, T));
	v_TBN = mat3(T, B, N);

	gl_Position =  viewProj * vec4(a_Position, 1.0);
}