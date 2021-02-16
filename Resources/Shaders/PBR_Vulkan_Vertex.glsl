#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in vec4 a_Color;

struct MaterialData
{
   ivec4 TextureStates;
   ivec4 TextureStates_2;

   ivec4 TextureIndexes;
   ivec4 TextureIndexes_2;

   vec4  PBRValues;
};

struct ShaderData
{
	mat4 model;
	vec4 addData;
};

struct SceneData
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

layout(std140, binding = 25) readonly buffer ShaderDataBuffer
{   
	ShaderData data[];

} shaderDataBuffer;

layout(std140, binding = 26) readonly buffer ObjectBuffer
{   
	MaterialData materials[];

} materialBuffer;

layout (std140, binding = 27) uniform SceneDataBuffer
{
    SceneData data;
} sceneData;

layout(push_constant) uniform CameraData
{
	int dataOffset;
};

layout (location = 0)  out vec3 outWorldPos;
layout (location = 1)  out vec3 outNormal;
layout (location = 2)  out vec3 outCameraPos;
layout (location = 3)  out vec2 outUV;
layout (location = 4)  out vec4 outTangent;

layout (location = 5)  out int outUseAlbedroMap;
layout (location = 6)  out int outUseNormalMap;
layout (location = 7)  out int outUseMetallicMap;
layout (location = 8)  out int outUseRoughnessMap;
layout (location = 9)  out int outUseAOMap;

layout (location = 10) out int outAlbedroMapIndex;
layout (location = 11) out int outNormalMapIndex;
layout (location = 12) out int outMetallicMapIndex;
layout (location = 13) out int outRoughnessMapIndex;
layout (location = 14) out int outAOMapIndex;

layout (location = 15) out float outMetallic;
layout (location = 16) out float outRoughness;
layout (location = 17) out vec4 outColor;
layout (location = 18) out mat3 outTBN;

void main()
{
	mat4 model = shaderDataBuffer.data[dataOffset + gl_InstanceIndex].model;
	int materialIndex = int(shaderDataBuffer.data[dataOffset + gl_InstanceIndex].addData.x);

	outWorldPos = vec3(model * vec4(a_Position, 1.0));
	outNormal =  mat3(model) * a_Normal;
	outTangent = vec4(mat3(model) * a_Tangent.xyz, a_Tangent.w);
	outUV = a_UV;
	outCameraPos = sceneData.data.camPos;

	// TBN matrix
	vec3 N = normalize(outNormal);
	vec3 T = normalize(outTangent.xyz);
	vec3 B = normalize(cross(N, T));
	outTBN = mat3(T, B, N);

	// PBR Params
	outMetallic = materialBuffer.materials[materialIndex].PBRValues.x;
	outRoughness = materialBuffer.materials[materialIndex].PBRValues.y;
	float c =  materialBuffer.materials[materialIndex].PBRValues.z;
	outColor = vec4(c, c, c, 1);

	// states
	outUseAlbedroMap = materialBuffer.materials[materialIndex].TextureStates.x;
	outUseNormalMap = materialBuffer.materials[materialIndex].TextureStates.y;
	outUseMetallicMap = materialBuffer.materials[materialIndex].TextureStates.z;
	outUseRoughnessMap = materialBuffer.materials[materialIndex].TextureStates.w;
	outUseAOMap = materialBuffer.materials[materialIndex].TextureStates_2.x;

	// index
	outAlbedroMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.x;
	outNormalMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.y;
	outMetallicMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.z;
	outRoughnessMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.w;
	outAOMapIndex = materialBuffer.materials[materialIndex].TextureIndexes_2.x;

	gl_Position =  sceneData.data.projection * sceneData.data.view * vec4(outWorldPos, 1.0);
}