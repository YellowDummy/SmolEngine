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

layout (binding = 24) uniform sampler2D texturesMap[4096];

layout (location = 0)  out vec3 outWorldPos;
layout (location = 1)  out vec3 outNormal;
layout (location = 2)  out vec3 outCameraPos;
layout (location = 3)  out vec2 outUV;
layout (location = 4)  out vec4 outTangent;

layout (location = 5) out float outMetallic;
layout (location = 6) out float outRoughness;
layout (location = 7) out float outExposure;
layout (location = 8) out float outGamma;
layout (location = 9) out float outAmbient;

layout (location = 10) out uint outDirectionalLightCount;
layout (location = 11) out uint outPointLightCount;

layout (location = 12) out vec3 outAO;
layout (location = 13) out vec3 outAlbedro;
layout (location = 14) out vec4 outShadowCoord;
layout (location = 15) out vec4 outRawPos;

void main()
{
	mat4 model = shaderDataBuffer.data[dataOffset + gl_InstanceIndex].model;
	int materialIndex = int(shaderDataBuffer.data[dataOffset + gl_InstanceIndex].data.x);

	outWorldPos = vec3(model * vec4(a_Position, 1.0));
	outTangent = vec4(mat3(model) * a_Tangent.xyz, a_Tangent.w);
	outCameraPos = sceneData.data.camPos.rgb;
	outUV = a_UV;
	outRawPos = vec4(a_Position, 1.0);

	//
	outExposure = sceneData.data.params.x;
	outGamma = sceneData.data.params.y;
	outAmbient = sceneData.data.params.z;

	//
	outDirectionalLightCount = directionalLights;
	outPointLightCount = pointLights;
	outShadowCoord = ( biasMat * lightSpace * model ) * vec4(a_Position, 1.0);	

	// states
	int useAlbedroMap = materialBuffer.materials[materialIndex].TextureStates.x;
	int useNormalMap = materialBuffer.materials[materialIndex].TextureStates.y;
	int useMetallicMap = materialBuffer.materials[materialIndex].TextureStates.z;
	int useRoughnessMap = materialBuffer.materials[materialIndex].TextureStates.w;
	int useAOMap = materialBuffer.materials[materialIndex].TextureStates_2.x;

	// index
	int albedroMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.x;
	int normalMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.y;
	int metallicMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.z;
	int roughnessMapIndex = materialBuffer.materials[materialIndex].TextureIndexes.w;
	int AOMapIndex = materialBuffer.materials[materialIndex].TextureIndexes_2.x;

	// TBN matrix
	vec3 modelNormal =  mat3(model) * a_Normal;
	vec3 N = normalize(modelNormal);
	vec3 T = normalize(outTangent.xyz);
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	outNormal = useNormalMap == 1? normalize(texture(texturesMap[normalMapIndex], a_UV).xyz * 2.0 - vec3(1.0)): TBN[2];

	// PBR Params
	float tempColor = materialBuffer.materials[materialIndex].PBRValues.z;
	outMetallic = useMetallicMap == 1 ? texture(texturesMap[metallicMapIndex], a_UV).r : materialBuffer.materials[materialIndex].PBRValues.x;
	outRoughness = useRoughnessMap == 1 ? texture(texturesMap[roughnessMapIndex], a_UV).r : materialBuffer.materials[materialIndex].PBRValues.y;
	outAlbedro = useAlbedroMap == 1 ? texture(texturesMap[albedroMapIndex], a_UV).rgb : vec3(tempColor, tempColor, tempColor);
	outAlbedro = pow(outAlbedro, vec3(2.2));
	outAO = useAOMap == 1 ? texture(texturesMap[AOMapIndex], a_UV).rrr : vec3(1.0, 1.0, 1.0);

	gl_Position =  sceneData.data.projection * sceneData.data.view * vec4(outWorldPos, 1.0);
}