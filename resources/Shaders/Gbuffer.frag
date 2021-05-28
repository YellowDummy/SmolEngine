#version 460

// In
layout (location = 0)  in vec3 v_FragPos;
layout (location = 1)  in vec3 v_Normal;
layout (location = 2)  in vec3 v_CameraPos;
layout (location = 3)  in vec2 v_UV;

layout (location = 4)  flat in uint v_UseAlbedroMap;
layout (location = 5)  flat in uint v_UseNormalMap;
layout (location = 6)  flat in uint v_UseMetallicMap;
layout (location = 7)  flat in uint v_UseRoughnessMap;
layout (location = 8)  flat in uint v_UseAOMap;

layout (location = 9)  flat in uint v_AlbedroMapIndex;
layout (location = 10) flat in uint v_NormalMapIndex;
layout (location = 11) flat in uint v_MetallicMapIndex;
layout (location = 12) flat in uint v_RoughnessMapIndex;
layout (location = 13) flat in uint v_AOMapIndex;

layout (location = 14) in float v_Metallic;
layout (location = 15) in float v_Roughness;

layout (location = 16) in vec4 v_Color;
layout (location = 17) in vec4 v_ShadowCoord;
layout (location = 18) in vec4 v_WorldPos;
layout (location = 19) in mat3 v_TBN;

layout (binding = 24) uniform sampler2D texturesMap[4096];


// Out
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 positions;
layout (location = 2) out vec4 normals;
layout (location = 3) out vec4 materials;
layout (location = 4) out vec4 shadowCoord;
layout (location = 5) out vec4 background;

vec3 getNormal()
{
	if(v_UseNormalMap == 1)
	{
		 vec3 tangentNormal = texture(texturesMap[v_NormalMapIndex], v_UV).xyz * 2.0 - 1.0;
	    return normalize(v_TBN * tangentNormal);
	}
	
	return normalize(v_Normal);
}

void main()
{
    vec3 N = getNormal(); 
    vec3 ao = v_UseAOMap == 1 ? texture(texturesMap[v_AOMapIndex], v_UV).rrr : vec3(1, 1, 1);
    vec3 albedro = v_UseAlbedroMap == 1 ? texture(texturesMap[v_AlbedroMapIndex], v_UV).rgb : v_Color.rgb;
    float metallic = v_UseMetallicMap == 1 ? texture(texturesMap[v_MetallicMapIndex], v_UV).r : v_Metallic;
	float roughness = v_UseRoughnessMap == 1 ? texture(texturesMap[v_RoughnessMapIndex], v_UV).r: v_Roughness;
	albedro = pow(albedro, vec3(2.2));


    color = vec4(albedro, 1.0);
    positions = vec4(v_FragPos, 1.0);
    normals = vec4(N, 1.0);
    materials = vec4(metallic, roughness, ao.x, 1.0);
    shadowCoord = v_ShadowCoord;
}