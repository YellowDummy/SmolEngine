#version 460 core

layout (location = 0)  in vec3 v_FragPos;
layout (location = 1)  in vec3 v_Normal;
layout (location = 2)  in vec2 v_UV;
layout (location = 3)  in vec3 v_Tangent;
layout (location = 4)  in vec3 v_CamPos;

layout (binding = 2) uniform samplerCube samplerIrradiance;
layout (binding = 3) uniform sampler2D samplerBRDFLUT;
layout (binding = 4) uniform samplerCube prefilteredMap;

layout (binding = 5) uniform sampler2D textures[6];

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

layout (std140, binding = 277) uniform SceneBuffer
{
	MaterialData material;
};

layout (location = 0) out vec4 outColor;

// From http://filmicgames.com/archives/75
vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

// PBR functions
// -----------------------------------------------------------------------------------------------------------------------
const float PI = 3.14159265359;
const float Epsilon = 0.00001;

float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}  

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 CalcIBL(vec3 N, vec3 V, vec3 F0, vec3 ao, vec3 albedo_color, float metallic_color, float roughness_color)
{
	float cosLo = max(dot(N, -V), 0.0);
	vec3 ReflDirectionWS = reflect(-V, N);
    vec3 F = fresnelSchlickRoughness(cosLo, F0, roughness_color);
	
    vec3 kD = (1.0 - F) * (1.0 - metallic_color);
    
    vec3 irradiance = texture(samplerIrradiance, -N).rgb;
	vec3 specularBRDF = texture(samplerBRDFLUT, vec2(cosLo, 1.0 - roughness_color)).rgb;
    vec3 diffuseIBL = irradiance * ( kD * albedo_color);
    
	int specularTextureLevels = textureQueryLevels(prefilteredMap);
	vec3 specularIrradiance =  textureLod(prefilteredMap, -ReflDirectionWS, roughness_color * specularTextureLevels).rgb;
    vec3 specularIBL = specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);
    
	return (diffuseIBL + specularIBL) * ao;
}

vec3 CalcDirLight(vec3 V, vec3 N, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color, vec3 modelPos)
{
	vec3 direction = vec3(30, 56, 0);
	vec3 L = normalize(direction.xyz - modelPos);
    vec3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0);       
	
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness_color);   
    float G   = GeometrySmith(N, V, L, roughness_color);      
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
	
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic_color);
	vec3 diffuseBRDF = kd * albedo_color;
	vec3 specularBRDF = (F * NDF * G) / max(Epsilon, 4.0 * NdotL * 0.8);
	
	return ((albedo_color / PI + diffuseBRDF + specularBRDF) * vec3(1.0) * NdotL)  * 5.0;
}

vec3 getNormal()
{
	if(material.UseNormalTex == 1)
	{
		vec3 tangentNormal = texture(textures[material.NormalTexIndex], v_UV).xyz * 2.0 - 1.0;
		// TBN matrix
	    vec3 T = normalize(v_Tangent);
	    vec3 N = normalize(v_Normal);
	    vec3 B = normalize(cross(N, T));
	    mat3 TBN = mat3(T, B, N);
	    return normalize(TBN * tangentNormal);
	}
	
	return normalize(v_Normal);
}

void main()
{
	// Getting Values 
	vec3 N = getNormal();
	vec3 albedro = material.UseAlbedroTex == 1 ? texture(textures[material.AlbedroTexIndex], v_UV).rgb : material.AlbedroColor.rgb;
	float metallic = material.UseMetallicTex == 1 ? texture(textures[material.MetallicTexIndex], v_UV).r : material.Metalness;
	float roughness = material.UseRoughnessTex == 1 ? texture(textures[material.RoughnessTexIndex], v_UV).r: material.Roughness;
	vec3 ao = material.UseAOTex == 1 ? texture(textures[material.AOTexIndex], v_UV).rrr : vec3(1);
	albedro = pow(albedro, vec3(2.2));
	vec3 F0 = mix(vec3(0.04), albedro, metallic); 
	vec3 V = normalize(v_CamPos - v_FragPos); 

    // Ambient Lighting (IBL)
	//--------------------------------------------
    vec3 ambient = CalcIBL(N, V, F0, ao, albedro, metallic, roughness);
	ambient *= 5;
	ambient += CalcDirLight(V, N, F0, albedro, metallic, roughness, v_FragPos);

	vec3 color = vec3(1.0) - exp(-ambient.rgb * 1.0);
	outColor = vec4(color, 1.0);
}