#version 450

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec4 inTangent;
layout (location = 4) in float inNearPlane;
layout (location = 5) in float inFarPlane;

layout (binding = 5) uniform sampler2D albedoMap;
layout (binding = 6) uniform sampler2D normalMap;
layout (binding = 7) uniform sampler2D aoMap;
layout (binding = 8) uniform sampler2D metallicMap;
layout (binding = 9) uniform sampler2D roughnessMap;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outPBR;
layout (location = 3) out vec4 outAlbedo;

vec3 calculateNormal()
{
	vec3 tangentNormal = texture(normalMap, inUV).xyz * 2.0 - 1.0;

	vec3 N = normalize(inNormal);
	vec3 T = normalize(inTangent.xyz);
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return TBN * normalize(texture(normalMap, inUV).xyz * 2.0 - vec3(1.0));
}

float linearDepth(float depth)
{
	float z = depth * 2.0f - 1.0f; 
	return (2.0f * inNearPlane * inFarPlane) / (inFarPlane + inNearPlane - z * (inFarPlane - inNearPlane));	
}

void main()
{		
	vec3 N = calculateNormal();
	float metallic = texture(metallicMap, inUV).r;
	float roughness = texture(roughnessMap, inUV).r;
    float ao = texture(aoMap, inUV).r;

	outAlbedo = texture(albedoMap, inUV);
    outNormal = vec4(N, 1.0);
    outPosition = vec4(inWorldPos, linearDepth(gl_FragCoord.z));

    outPBR.x =  metallic;
	outPBR.y =  roughness;
	outPBR.z = ao;
}