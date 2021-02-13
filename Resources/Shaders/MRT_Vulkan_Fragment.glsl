#version 450

// in

layout (location = 0)  in vec3 inWorldPos;
layout (location = 1)  in vec3 inNormal;
layout (location = 2)  in vec2 inUV;
layout (location = 3)  in vec4 inTangent;
layout (location = 4)  in vec4 inColor;

layout (location = 5)  in float inNearPlane;
layout (location = 6)  in float inFarPlane;

layout (location = 7)  flat in int inUseAlbedroMap;
layout (location = 8)  flat in int inUseNormalMap;
layout (location = 9)  flat in int inUseMetallicMap;
layout (location = 10) flat in int inUseRoughnessMap;
layout (location = 11) flat in int inUseAOMap;

layout (location = 12) flat in int inAlbedroMapIndex;
layout (location = 13) flat in int inNormalMapIndex;
layout (location = 14) flat in int inMetallicMapIndex;
layout (location = 15) flat in int inRoughnessMapIndex;
layout (location = 16) flat in int inAOMapIndex;

layout (location = 17) in float inMetallic;
layout (location = 18) in float inRoughness;
layout (location = 19) in float inPDepth;

layout (location = 20) in mat3 inTBN;

// uniforms

layout (binding = 5) uniform sampler2D albedoMap[256];
layout (binding = 6) uniform sampler2D normalMap[256];
layout (binding = 7) uniform sampler2D aoMap[256];
layout (binding = 8) uniform sampler2D metallicMap[256];
layout (binding = 9) uniform sampler2D roughnessMap[256];

// out

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outPBR;
layout (location = 3) out vec4 outAlbedo;

vec3 calculateNormal()
{
    if(inUseNormalMap == 1)
	{
		vec3 tangentNormal = texture(normalMap[inNormalMapIndex], inUV).xyz * 2.0 - 1.0;
		return normalize(inTBN * tangentNormal);
	}
	else
	{
		return normalize(inTBN[2]);
	}
}

void main()
{		
	vec3 N = calculateNormal();
	float metallic = inUseMetallicMap == 1? texture(metallicMap[inMetallicMapIndex], inUV).r : inMetallic;
	float roughness = inUseRoughnessMap == 1 ? texture(roughnessMap[inRoughnessMapIndex], inUV).r : inRoughness;
    float ao = inUseAOMap == 1? texture(aoMap[inAOMapIndex], inUV).r : 1.0;

	outAlbedo = inUseAlbedroMap == 1? texture(albedoMap[inAlbedroMapIndex], inUV) : inColor;
    outNormal = vec4(N, 1);
    outPosition = vec4(inWorldPos, inPDepth);

    outPBR.x =  metallic;
	outPBR.y =  roughness;
	outPBR.z = ao;
	outPBR.w = inUseAOMap; // ssao 
}