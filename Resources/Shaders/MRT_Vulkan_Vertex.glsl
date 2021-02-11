#version 460 core

layout(location = 0)  in vec3 a_Position;
layout(location = 1)  in vec3 a_Normal;
layout(location = 2)  in vec4 a_Tangent;
layout(location = 3)  in vec2 a_UV;
layout(location = 4)  in vec4 a_Color;

struct MaterialData
{
   int UseAlbedroMap;
   int UseNormalMap;
   int UseMetallicMap;
   int UseRoughnessMap;
   int UseAOMap;
   
   int AlbedroMapIndex;
   int NormalMapIndex;
   int MetallicMapIndex;
   int RoughnessMapIndex;
   int AOMapIndex;
   
   float Metallic;
   float Roughness;
   float Ambient;
   float Albedo;
   float Specular;
};

struct ModelData
{
	mat4 model;
};

// all object matrices
layout(std140, binding = 25) readonly buffer ModelBuffer
{   
	ModelData models[];

} modelsBuffer;

// all object materials
layout(std140, binding = 26) readonly buffer ObjectBuffer
{   
	MaterialData materials[];

} materialBuffer;

layout(push_constant) uniform CameraData
{
	mat4 projection;
	mat4 view;

	float nearPlane;
	float farPlane;

	int modelIndex;
	int materialIndex;
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
	mat4 model = modelsBuffer.models[modelIndex].model;

	outWorldPos = vec3(model * vec4(a_Position, 1.0));
	outNormal =  vec3(mat3(model) * a_Normal);
	outTangent = vec4(mat3(model) * a_Tangent.xyz, a_Tangent.w);
	outUV = a_UV;
	
	float c =  materialBuffer.materials[materialIndex].Albedo;
	outColor = vec4(c, c, c, 1);

	// TBN matrix

	vec3 N = normalize(outNormal);
	vec3 T = normalize(outTangent.xyz);
	vec3 B = normalize(cross(N, T));
	outTBN = mat3(T, B, N);

	outNearPlane = nearPlane;
	outFarPlane = farPlane;
	outRoughness = materialBuffer.materials[materialIndex].Roughness;
	outMetallic = materialBuffer.materials[materialIndex].Metallic;

	// states
	outUseAlbedroMap = materialBuffer.materials[materialIndex].UseAlbedroMap;
	outUseNormalMap = materialBuffer.materials[materialIndex].UseNormalMap;
	outUseMetallicMap = materialBuffer.materials[materialIndex].UseMetallicMap;
	outUseRoughnessMap = materialBuffer.materials[materialIndex].UseRoughnessMap;
	outUseAOMap = materialBuffer.materials[materialIndex].UseAOMap;

	// index
	outAlbedroMapIndex = materialBuffer.materials[gl_InstanceIndex].AlbedroMapIndex;
	outNormalMapIndex = materialBuffer.materials[gl_InstanceIndex].NormalMapIndex;
	outMetallicMapIndex = materialBuffer.materials[gl_InstanceIndex].MetallicMapIndex;
	outRoughnessMapIndex = materialBuffer.materials[gl_InstanceIndex].RoughnessMapIndex;
	outAOMapIndex = materialBuffer.materials[gl_InstanceIndex].AOMapIndex;

	gl_Position =  projection * view * vec4(outWorldPos, 1.0);
}