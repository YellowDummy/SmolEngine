#version 460

// Buffers
// -----------------------------------------------------------------------------------------------------------------------

struct MaterialData
{
	vec4 AlbedroColor;

	float Metalness;
	float Roughness;
	float EmissionStrength;
	uint UseAlbedroTex;

	uint UseNormalTex;
	uint UseMetallicTex;
	uint UseRoughnessTex;
    uint UseAOTex;

	uint UseEmissiveTex;
	uint AlbedroTexIndex;
	uint NormalTexIndex;
	uint MetallicTexIndex;

	uint RoughnessTexIndex;
	uint AOTexIndex;
	uint EmissiveTexIndex;
};

// In
layout (location = 0)  in vec3 v_FragPos;
layout (location = 1)  in vec3 v_Normal;
layout (location = 2)  in vec3 v_CameraPos;
layout (location = 3)  in vec2 v_UV;
layout (location = 4)  in vec4 v_ShadowCoord;
layout (location = 5)  in vec4 v_WorldPos;
layout (location = 6)  in vec3 v_Tangent;
layout (location = 7)  in float v_Depth;
layout (location = 8)  flat in MaterialData v_Material;

// Out
layout (location = 0) out vec4 out_color;
layout (location = 1) out vec4 out_positions;
layout (location = 2) out vec4 out_normals;
layout (location = 3) out vec4 out_materials;
layout (location = 4) out vec4 out_shadowCoord;

layout (binding = 24) uniform sampler2D texturesMap[4096];

vec3 linearFromSRGB(vec3 sRGB) 
{
    return pow(sRGB, vec3(2.2));
}

vec3 fetchAlbedoMap() 
{
    return texture(texturesMap[v_Material.AlbedroTexIndex], v_UV).rgb;
}

vec3 fetchNormalMap() 
{
	if(v_Material.UseNormalTex == 1)
	{  
       vec3 normal = texture(texturesMap[v_Material.NormalTexIndex], v_UV).xyz;
	   // TBN matrix
	   vec3 B = normalize(vec3(vec4(cross(v_Normal, v_Tangent), 0.0)));
	   mat3 TBN = mat3(v_Tangent, B, v_Normal);
       return normalize(TBN * (normal * 2.0 - 1.0));
	}
	else
	{
		return normalize(v_Normal);
	}
}

float fetchMetallicMap() 
{
    return texture(texturesMap[v_Material.MetallicTexIndex], v_UV).r;
}

float fetchRoughnessMap() 
{
    return texture(texturesMap[v_Material.RoughnessTexIndex], v_UV).r;
}

float fetchEmissiveMap() 
{
    return texture(texturesMap[v_Material.EmissiveTexIndex], v_UV).r;
}

float fetchAOMap() 
{
    return texture(texturesMap[v_Material.AOTexIndex], v_UV).r;
}

float FetchDisplacementMap() 
{
    return 0;
}


void main()
{
	vec3 N = fetchNormalMap(); 		
	vec4 albedro = v_Material.UseAlbedroTex == 1 ? vec4(fetchAlbedoMap(), 1) : v_Material.AlbedroColor;
	float emissive = v_Material.UseEmissiveTex == 1 ? fetchEmissiveMap() : float(v_Material.EmissionStrength);
	float metallic = v_Material.UseMetallicTex == 1 ? fetchMetallicMap() : v_Material.Metalness;
	float roughness = v_Material.UseRoughnessTex == 1 ? fetchRoughnessMap() : v_Material.Roughness;
    float ao = v_Material.UseAOTex == 1 ? fetchAOMap() : 1.0;				

    out_color = albedro;
    out_positions = vec4(v_FragPos, v_Depth);
    out_normals = vec4(N, float(v_Material.UseAOTex));
    out_materials = vec4(metallic, roughness, ao, emissive);
    out_shadowCoord = v_ShadowCoord;
}