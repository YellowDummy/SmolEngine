#version 460 core
layout (location = 0)  in vec3 v_FragPos;
layout (location = 1)  in vec3 v_Normal;
layout (location = 2)  in vec2 v_UV;
layout (location = 3)  in vec4 v_Color;
layout (location = 4)  in vec4 v_WorldPos;
layout (location = 5)  in mat3 v_TBN;

layout (binding = 2) uniform samplerCube samplerIrradiance;
layout (binding = 3) uniform sampler2D samplerBRDFLUT;
layout (binding = 4) uniform samplerCube prefilteredMap;

layout (binding = 5) uniform sampler2D albedroMap;
layout (binding = 6) uniform sampler2D normalMap;
layout (binding = 7) uniform sampler2D metallicMap;
layout (binding = 8) uniform sampler2D roughnessMap;
layout (binding = 9) uniform sampler2D aoMap;

layout (std140, binding = 277) uniform SceneBuffer
{
	uint useAlbedro;
	uint useNormal;
	uint useRoughness;
	uint useMetallic;

	uint useAO;
	vec3 camPos;
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

vec3 getNormal()
{
	if(useNormal == 1)
	{
		vec3 tangentNormal = texture(normalMap, v_UV).xyz * 2.0 - 1.0;
	    return normalize(v_TBN * tangentNormal);
	}
	
	return normalize(v_Normal);
}

void main()
{
	// Getting Values 
	vec3 N = getNormal();
	vec3 ao = useAO == 1 ? texture(aoMap, v_UV).rrr : vec3(1, 1, 1);
	vec3 albedro = useAlbedro == 1 ? texture(albedroMap, v_UV).rgb : vec3(1.0, 1.0, 1.0);
	float metallic = useMetallic == 1 ? texture(metallicMap, v_UV).r : 0.2;
	float roughness = useRoughness == 1 ? texture(roughnessMap, v_UV).r: 1.0;

	albedro = pow(albedro, vec3(2.2));

    // Ambient Lighting (IBL)
	//--------------------------------------------
	vec3 F0 = mix(vec3(0.04), albedro, metallic); 
	vec3 V = normalize(camPos - v_FragPos); 
    vec3 ambient = CalcIBL(N, V, F0, ao, albedro, metallic, roughness);

    // Tone mapping
	ambient = Uncharted2Tonemap(ambient * 4.0);
	ambient = ambient * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	ambient = pow(ambient, vec3(1.0f / 2.5));

	outColor = vec4(ambient, 1.0);
}