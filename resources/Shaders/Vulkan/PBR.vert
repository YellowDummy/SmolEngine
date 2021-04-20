#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in ivec4 a_BoneIDs;
layout(location = 5) in vec4 a_Weight;

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
	vec4 data;
};

struct SceneData
{
	mat4 projection;
	mat4 view;
	mat4 skyBoxMatrix;
	vec4 camPos;
	vec4 params;
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

layout(push_constant) uniform ConstantData
{
	mat4 lightSpace;

	uint dataOffset;
	uint directionalLights;
	uint pointLights;
};

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, -0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

layout (location = 0)  out vec3 v_ModelPos;
layout (location = 1)  out vec3 v_Normal;
layout (location = 2)  out vec3 v_CameraPos;
layout (location = 3)  out vec2 v_UV;

layout (location = 4)  out uint v_UseAlbedroMap;
layout (location = 5)  out uint v_UseNormalMap;
layout (location = 6)  out uint v_UseMetallicMap;
layout (location = 7)  out uint v_UseRoughnessMap;
layout (location = 8)  out uint v_UseAOMap;

layout (location = 9)  out uint v_AlbedroMapIndex;
layout (location = 10) out uint v_NormalMapIndex;
layout (location = 11) out uint v_MetallicMapIndex;
layout (location = 12) out uint v_RoughnessMapIndex;
layout (location = 13) out uint v_AOMapIndex;

layout (location = 14) out float v_Metallic;
layout (location = 15) out float v_Roughness;
layout (location = 16) out float v_Exposure;
layout (location = 17) out float v_Gamma;
layout (location = 18) out float v_Ambient;

layout (location = 19) out uint v_DirectionalLightCount;
layout (location = 20) out uint v_PointLightCount;

layout (location = 21) out vec4 v_Color;
layout (location = 22) out vec4 v_ShadowCoord;
layout (location = 23) out vec4 v_WorldPos;
layout (location = 24) out mat3 v_TBN;

void main()
{
	mat4 model = shaderDataBuffer.data[dataOffset + gl_InstanceIndex].model;
	int materialIndex = int(shaderDataBuffer.data[dataOffset + gl_InstanceIndex].data.x);

	v_ModelPos = vec3(model * vec4(a_Position, 1.0));
	v_Normal =  mat3(model) * a_Normal;
	v_CameraPos = sceneData.data.camPos.rgb;
	v_UV = a_UV;
	v_Exposure = sceneData.data.params.x;
	v_Gamma = sceneData.data.params.y;
	v_Ambient = sceneData.data.params.z;
	v_DirectionalLightCount = directionalLights;
	v_PointLightCount = pointLights;
	v_ShadowCoord = ( biasMat * lightSpace * model ) * vec4(a_Position, 1.0);	
	v_WorldPos = vec4(a_Position, 1.0);

	// TBN matrix
	vec4 modelTangent = vec4(mat3(model) * a_Tangent.xyz, 1.0);
	vec3 N = normalize(v_Normal);
	vec3 T = normalize(modelTangent.xyz);
	vec3 B = normalize(cross(N, T));
	v_TBN = mat3(T, B, N);

	// PBR Params
	v_Metallic = materialBuffer.materials[materialIndex].PBRValues.x;
	v_Roughness = materialBuffer.materials[materialIndex].PBRValues.y;
	float c =  materialBuffer.materials[materialIndex].PBRValues.z;
	v_Color = vec4(c, c, c, 1);

	// states
	v_UseAlbedroMap = materialBuffer.materials[materialIndex].TextureStates.x;
	v_UseNormalMap = materialBuffer.materials[materialIndex].TextureStates.y;
	v_UseMetallicMap = materialBuffer.materials[materialIndex].TextureStates.z;
	v_UseRoughnessMap = materialBuffer.materials[materialIndex].TextureStates.w;
	v_UseAOMap = materialBuffer.materials[materialIndex].TextureStates_2.x;

	// index
	v_AlbedroMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.x;
	v_NormalMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.y;
	v_MetallicMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.z;
	v_RoughnessMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.w;
	v_AOMapIndex = materialBuffer.materials[materialIndex].TextureIndexes_2.x;

	gl_Position =  sceneData.data.projection * sceneData.data.view * vec4(v_ModelPos, 1.0);
}