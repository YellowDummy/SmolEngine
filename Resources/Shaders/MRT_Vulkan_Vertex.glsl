#version 460 core

// binding 1 - mesh
layout(location = 0)  in vec3 a_Position;
layout(location = 1)  in vec3 a_Normal;
layout(location = 2)  in vec4 a_Tangent;
layout(location = 3)  in vec2 a_UV;
layout(location = 4)  in vec4 a_Color;

// binding 2 - instance input rate
layout(location = 5)  in int a_UseAlbedroMap;
layout(location = 6)  in int a_UseNormalMap;
layout(location = 7)  in int a_UseMetallicMap;
layout(location = 8)  in int a_UseRoughnessMap;
layout(location = 9)  in int a_UseAOMap;

layout(location = 10) in int a_AlbedroMapIndex;
layout(location = 11) in int a_NormalMapIndex;
layout(location = 12) in int a_MetallicMapIndex;
layout(location = 13) in int a_RoughnessMapIndex;
layout(location = 14) in int a_AOMapIndex;

layout(location = 15) in float a_Metallic;
layout(location = 16) in float a_Roughness;

struct InstanceData
{
	mat4 model;
};

//all object matrices
layout(std140, binding = 25) readonly buffer ObjectBuffer
{   
	InstanceData objects[];

} objectBuffer;

layout(push_constant) uniform CameraData
{
	mat4 projection;
	mat4 view;

	float nearPlane;
	float farPlane;
};

// out values

layout (location = 0)  out vec3 outWorldPos;
layout (location = 1)  out vec3 outNormal;
layout (location = 2)  out vec2 outUV;
layout (location = 3)  out vec4 outTangent;
layout (location = 4)  out vec4 outColor;

layout (location = 6)  out float outNearPlane;
layout (location = 7)  out float outFarPlane;

layout (location = 8)  out int outUseAlbedroMap;
layout (location = 9)  out int outUseNormalMap;
layout (location = 10) out int outUseMetallicMap;
layout (location = 11) out int outUseRoughnessMap;
layout (location = 12) out int outUseAOMap;

layout (location = 13) out int outAlbedroMapIndex;
layout (location = 14) out int outNormalMapIndex;
layout (location = 15) out int outMetallicMapIndex;
layout (location = 16) out int outRoughnessMapIndex;
layout (location = 17) out int outAOMapIndex;

layout (location = 18)  out float outMetallic;
layout (location = 19)  out float outRoughness;

layout (location = 20)  out mat3 outTBN;

void main()
{
	mat4 model = objectBuffer.objects[gl_InstanceIndex].model;

	outWorldPos = vec3(model  * vec4(a_Position, 1.0));
	outNormal = mat3(model) * a_Normal;
	outTangent = vec4(mat3(model) * a_Tangent.xyz, a_Tangent.w);
	outUV = a_UV;

	// TBN matrix

	vec3 N = normalize(outNormal);
	vec3 T = normalize(outTangent.xyz);
	vec3 B = normalize(cross(N, T));
	outTBN = mat3(T, B, N);

	outNearPlane = nearPlane;
	outFarPlane = farPlane;
	outRoughness = a_Roughness;
	outMetallic = a_Metallic;

	// states
	outUseAlbedroMap = a_UseAlbedroMap;
	outUseNormalMap = a_UseNormalMap;
	outUseMetallicMap = a_UseMetallicMap;
	outUseRoughnessMap = a_UseRoughnessMap;
	outUseAOMap = a_UseAOMap;

	// index
	outAlbedroMapIndex = a_AlbedroMapIndex;
	outNormalMapIndex = a_NormalMapIndex;
	outMetallicMapIndex = a_MetallicMapIndex;
	outRoughnessMapIndex = a_RoughnessMapIndex;
	outAOMapIndex = a_AOMapIndex;

	gl_Position =  projection * view * vec4(outWorldPos, 1.0);
}