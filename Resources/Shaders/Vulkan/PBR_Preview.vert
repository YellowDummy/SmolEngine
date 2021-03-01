#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in vec4 a_Color;

layout (location = 0)  out vec3 ModelWorldPos;
layout (location = 1)  out vec3 Normal;
layout (location = 2)  out vec2 UV;
layout (location = 3)  out vec3 WorldPos;
layout (location = 4)  out mat3 TBN;

layout(push_constant) uniform ConstantData
{
    mat4 viewProj;
};

void main()
{
    mat4 model = mat4(1.0);

    WorldPos = a_Position;
    ModelWorldPos = vec3(model * vec4(a_Position, 1.0));
	Normal =  mat3(model) * a_Normal;
	UV = a_UV;

    vec4 modelTangent = vec4(mat3(model) * a_Tangent.xyz, a_Tangent.w);
	vec3 N = normalize(Normal);
	vec3 T = normalize(modelTangent.xyz);
	vec3 B = normalize(cross(N, T));
	TBN = mat3(T, B, N);

    gl_Position =  viewProj * vec4(ModelWorldPos, 1.0);
}