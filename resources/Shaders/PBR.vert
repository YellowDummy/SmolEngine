#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in ivec4 a_BoneIDs;
layout(location = 5) in vec4 a_Weight;

struct MaterialData
{
	vec4 AlbedroColor;

	float Metalness;
	float Roughness;
	uint UseAlbedroTex;
	uint UseNormalTex;

	uint UseMetallicTex;
	uint UseRoughnessTex;
	uint UseEmissiveTex;
	uint UseHeightTex;

	uint UseAOTex;
	uint AlbedroTexIndex;
	uint NormalTexIndex;
	uint MetallicTexIndex;

	uint RoughnessTexIndex;
	uint AOTexIndex;
	uint EmissiveTexIndex;
	uint HeightTexIndex;
};

struct InstanceData
{
	uint matID;
	uint isAnimated;
	uint animOffset;
	uint entityID;
	mat4 model;
};

layout(std140, binding = 25) readonly buffer InstancesBuffer
{   
	InstanceData instances[];
};

layout(std140, binding = 26) readonly buffer MaterialsBuffer
{   
	MaterialData materials[];
};

layout (std140, binding = 27) uniform SceneBuffer
{
	float nearClip;
    float farClip;
    float exoposure;
    float pad;

	mat4 projection;
	mat4 view;
	mat4 skyBoxMatrix;
	vec4 camPos;
	vec4 ambientColor;

} sceneData;

layout(std430, binding = 28) readonly buffer JointMatrices
{
	mat4 joints[];
};

layout(push_constant) uniform ConstantData
{
	mat4 lightSpace;
	uint dataOffset;
};

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, -0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

layout (location = 0)  out vec3 v_FragPos;
layout (location = 1)  out vec3 v_Normal;
layout (location = 2)  out vec3 v_CameraPos;
layout (location = 3)  out vec2 v_UV;
layout (location = 4)  out vec4 v_ShadowCoord;
layout (location = 5)  out vec4 v_WorldPos;
layout (location = 6)  out MaterialData v_Material;
layout (location = 23) out mat3 v_TBN;

void main()
{
	const uint instanceID = dataOffset + gl_InstanceIndex;
	const mat4 model = instances[instanceID].model;
	const uint materialIndex = instances[instanceID].matID;
	const uint animOffset = instances[instanceID].animOffset;
	const bool isAnimated = bool(instances[instanceID].isAnimated);

	mat4 skinMat = mat4(1.0);
	if(isAnimated)
	{
		skinMat = 
		a_Weight.x * joints[animOffset + uint(a_BoneIDs.x)] +
		a_Weight.y * joints[animOffset + uint(a_BoneIDs.y)] +
		a_Weight.z * joints[animOffset + uint(a_BoneIDs.z)] +
		a_Weight.w * joints[animOffset + uint(a_BoneIDs.w)];
	}

	const mat4 modelSkin = model * skinMat;
	v_FragPos = vec3(modelSkin *  vec4(a_Position, 1.0));
	v_Normal =  mat3(transpose(inverse(modelSkin))) * a_Normal;
	v_CameraPos = sceneData.camPos.xyz;
	v_ShadowCoord = ( biasMat * lightSpace * modelSkin) * vec4(a_Position, 1.0);	
	v_WorldPos = vec4(a_Position, 1.0);
	v_UV = a_UV;

	// Materials
	v_Material =  materials[materialIndex];

	// TBN matrix
	vec3 T = normalize(vec3(modelSkin * vec4(a_Tangent, 0.0)));
	vec3 N = normalize(vec3(modelSkin * vec4(a_Normal, 0.0)));
	vec3 B = normalize(vec3(modelSkin * vec4(cross(N, T), 0.0)));
	v_TBN = mat3(T, B, N);

	gl_Position =  sceneData.projection * sceneData.view * model * skinMat * vec4(a_Position, 1.0);
}