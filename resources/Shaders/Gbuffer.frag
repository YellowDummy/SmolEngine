#version 460

// Buffers
// -----------------------------------------------------------------------------------------------------------------------

struct MaterialData
{
	vec4 AlbedroColor;

	float Metalness;
	float Roughness;
	uint UseAlbedroTex;
	uint UseNormalTex;

	uint UseMetallicTex;
	uint UseRoughnessTex;
    uint UseAOTex;
	uint UseEmissiveTex;

	uint UseHeightTex;
	uint AlbedroTexIndex;
	uint NormalTexIndex;
	uint MetallicTexIndex;

	uint RoughnessTexIndex;
	uint AOTexIndex;
	uint EmissiveTexIndex;
	uint HeightTexIndex;
};

// In
layout (location = 0)  in vec3 v_FragPos;
layout (location = 1)  in vec3 v_Normal;
layout (location = 2)  in vec3 v_CameraPos;
layout (location = 3)  in vec2 v_UV;
layout (location = 4)  in vec4 v_ShadowCoord;
layout (location = 5)  in vec4 v_WorldPos;
layout (location = 6)  in vec3 v_Tangent;
layout (location = 7)  flat in MaterialData v_Material;

// Out
layout (location = 0) out vec4 out_color;
layout (location = 1) out vec4 out_positions;
layout (location = 2) out vec4 out_normals;
layout (location = 3) out vec4 out_materials;
layout (location = 4) out vec4 out_emission;
layout (location = 5) out vec4 out_shadowCoord;

layout (binding = 24) uniform sampler2D texturesMap[4096];

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 getNormal()
{
    if(v_Material.UseNormalTex == 1)
    {
        // Perturb normal, see http://www.thetenthplanet.de/archives/1180
	    vec3 tangentNormal = texture(texturesMap[v_Material.NormalTexIndex], v_UV).xyz * 2.0 - 1.0;

	    // TBN matrix
	    vec3 B = normalize(vec3(vec4(cross(v_Normal, v_Tangent), 0.0)));
	    mat3 TBN = mat3(v_Tangent, B, v_Normal);
        return normalize(TBN * tangentNormal);
    }

    return normalize(v_Normal);
}

void main()
{
	vec4 albedro = v_Material.UseAlbedroTex == 1 ? texture(texturesMap[v_Material.AlbedroTexIndex], v_UV) : v_Material.AlbedroColor;
	vec4 emissive = v_Material.UseEmissiveTex == 1 ? texture(texturesMap[v_Material.EmissiveTexIndex], v_UV): vec4(0.0);
	float metallic = v_Material.UseMetallicTex == 1 ? texture(texturesMap[v_Material.MetallicTexIndex], v_UV).r : v_Material.Metalness;
	float roughness = v_Material.UseRoughnessTex == 1 ? texture(texturesMap[v_Material.RoughnessTexIndex], v_UV).r: v_Material.Roughness;
    float ao = v_Material.UseAOTex == 1 ? texture(texturesMap[v_Material.AOTexIndex], v_UV).r : 1.0;

    vec3 N = getNormal(); 						
	vec3 V = normalize(v_CameraPos - v_FragPos);
    float applyEmission = float(v_Material.UseEmissiveTex);
    float applyAO = float(v_Material.UseAOTex);

    out_color = albedro;
    out_positions = vec4(v_FragPos, applyEmission);
    out_normals = vec4(N, applyAO);
    out_materials = vec4(metallic, roughness, ao, 1.0);
    out_shadowCoord = v_ShadowCoord;
	out_emission = vec4(emissive.rgb, applyEmission);
}