#version 450
#define ambient 0.1

// In
layout (location = 0)  in vec3 v_ModelPos;
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
layout (location = 16) in float v_Exposure;
layout (location = 17) in float v_Gamma;
layout (location = 18) in float v_Ambient;

layout (location = 19) flat in uint v_DirectionalLightCount;
layout (location = 20) flat in uint v_PointLightCount;

layout (location = 21) in vec4 v_Color;
layout (location = 22) in vec4 v_ShadowCoord;
layout (location = 23) in vec4 v_WorldPos;
layout (location = 24) in mat3 v_TBN;

// Out
layout (location = 0) out vec4 outColor;

// Samplers
layout (binding = 1) uniform sampler2D shadowMap;
layout (binding = 2) uniform samplerCube samplerIrradiance;
layout (binding = 3) uniform sampler2D samplerBRDFLUT;
layout (binding = 4) uniform samplerCube prefilteredMap;
layout (binding = 24) uniform sampler2D texturesMap[4096];

// Buffers
struct DirectionalLightBuffer
{
    vec4 position;
	vec4 color;
};

struct AmbientLightingBuffer
{
	vec4 diffuseColor;
	vec4 specularColor;
	vec4 ambientColor;
	vec4 params; // x = IBL scale, y = enable IBL;
};

struct PointLightBuffer
{
    vec4 position;
	vec4 color;
	vec4 params;
};

layout(std140, binding = 28) readonly buffer DirectionalLightStorage
{   
	DirectionalLightBuffer directionalLights[];
};

layout(std140, binding = 29) readonly buffer PointLightStorage
{   
	PointLightBuffer pointLights[];
};

layout(std140, binding = 30) uniform AmbientLightingStorage
{   
	AmbientLightingBuffer ambientLightingData;
};

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);
const float PI = 3.141592;
const float Epsilon = 0.00001;

float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( shadowMap, shadowCoord.st + off ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z ) 
		{
			shadow = ambient;
		}
	}
	return shadow;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(shadowMap, 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	
	}
	return shadowFactor / count;
}

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

vec4 tonemap(vec4 color)
{
	vec3 outcol = Uncharted2Tonemap(color.rgb * v_Exposure);
	outcol = outcol * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	return vec4(pow(outcol, vec3(1.0f / v_Gamma)), color.a);
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 getNormal()
{
    if(v_UseNormalMap == 1)
	{
		return v_TBN * normalize(texture(texturesMap[v_NormalMapIndex], v_UV).xyz * 2.0 - vec3(1.0));
	}

	return v_TBN[2];
}

void main()
{	
	// Getting Values 
	vec3 N = getNormal();
	vec3 ao = v_UseAOMap == 1 ? texture(texturesMap[v_AOMapIndex], v_UV).rrr : vec3(1, 1, 1);
	vec3 albedro = v_UseAlbedroMap == 1 ? texture(texturesMap[v_AlbedroMapIndex], v_UV).rgb : v_Color.rgb;
	albedro = pow(albedro, vec3(2.2));

	float metallic = v_UseMetallicMap == 1 ? texture(texturesMap[v_MetallicMapIndex], v_UV).r : v_Metallic;
	float roughness = v_UseRoughnessMap == 1 ? texture(texturesMap[v_RoughnessMapIndex], v_UV).r: v_Roughness;
	
	// Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(v_CameraPos - v_ModelPos);

	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));

	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedro, metallic);

    // Ambient lighting (IBL).
	vec3 ambientLighting = ambientLightingData.ambientColor.rgb;
	if(ambientLightingData.params.y == 1.0)
	{
		vec3 F = fresnelSchlick(F0, cosLo);
		// Get diffuse contribution factor (as with direct lighting).
		vec3 kd = mix(vec3(1.0), F, metallic);

		// Sample diffuse irradiance at normal direction.
		vec3 irradiance = texture(samplerIrradiance, -N).rgb;
		// Split-sum approximation factors for Cook-Torrance specular BRDF.
		vec3 specularBRDF = texture(samplerBRDFLUT, vec2(cosLo, 1.0 - roughness)).rgb;
		// Sample pre-filtered specular reflection environment at correct mipmap level.
		int specularTextureLevels = textureQueryLevels(prefilteredMap);
		vec3 specularIrradiance =  textureLod(prefilteredMap, -Lr, roughness * specularTextureLevels).rgb;
		
		// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
		vec3 diffuseIBL = irradiance * ( kd * albedro * ambientLightingData.diffuseColor.rgb);

		// Total specular IBL contribution.
		vec3 specularIBL = specularIrradiance * (F0 * ambientLightingData.specularColor.rgb * specularBRDF.x + specularBRDF.y);

		diffuseIBL *= ambientLightingData.params.x; // x = IBL scale
		specularIBL *= ambientLightingData.params.x;

		// Total ambient lighting contribution.
		ambientLighting = diffuseIBL + specularIBL;
	}
	
	// Direct lighting calculation for analytical lights.
	vec3 directLighting = vec3(0);
	for(int i=0; i< v_DirectionalLightCount; ++i)
	{
		vec3 L = normalize(directionalLights[0].position.xyz - v_WorldPos.xyz);
		vec3 Lradiance = directionalLights[i].color.rgb;

		// Half-vector between Li and Lo.
		vec3 Lh = normalize(L + Lo);

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(N, L));
		float cosLh = max(0.0, dot(N, Lh));

		// Calculate Fresnel term for direct lighting. 
		vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		// Calculate normal distribution for specular BRDF.
		float D = ndfGGX(cosLh, roughness);
		// Calculate geometric attenuation for specular BRDF.
		float G = gaSchlickGGX(cosLi, cosLo, roughness);

		// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
		// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
		// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

		// Lambert diffuse BRDF.
		// We don't scale by 1/PI for lighting & material units to be more convenient.
		// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
		vec3 diffuseBRDF = kd * albedro;

		// Cook-Torrance specular microfacet BRDF.
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * 0.8);
		vec3 result = (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
		// Total contribution for this light.
		directLighting += result * 2.0; // temp
	}

	// Point lighting calculation
	vec3 pointLighting = vec3(0);
	for(int i=0; i< v_PointLightCount; ++i)
	{
		float constantAtt = pointLights[i].params.x;
		float linearAtt = pointLights[i].params.y;
		float expAtt = pointLights[i].params.z;

	    vec3 posToLight = v_ModelPos - pointLights[i].position.xyz;
	    float dist = length(posToLight);
	    posToLight = normalize(posToLight);
		float diffuse = max(0.0, dot(N, -posToLight));

		float attTotal = constantAtt + linearAtt * dist + expAtt * dist * dist;
		pointLighting += pointLights[i].color.rgb * (ambientLighting + diffuse) / attTotal;
	}

	vec3 color = directLighting + pointLighting;
	color += ambientLighting;
	if(v_DirectionalLightCount > 0)
	{
		float shadow = filterPCF(v_ShadowCoord / v_ShadowCoord.w);
		color*= shadow;
	}
	// Tone mapping
	color = Uncharted2Tonemap(color * v_Exposure);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / v_Gamma));

	outColor = vec4(color, 1.0);
}