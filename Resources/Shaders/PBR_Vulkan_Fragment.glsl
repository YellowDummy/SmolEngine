#version 450

layout (location = 0)  in vec3 inWorldPos;
layout (location = 1)  in vec3 inNormal;
layout (location = 2)  in vec3 inCameraPos;
layout (location = 3)  in vec2 inUV;
layout (location = 4)  in vec4 inTangent;

layout (location = 5)  flat in int inUseAlbedroMap;
layout (location = 6)  flat in int inUseNormalMap;
layout (location = 7)  flat in int inUseMetallicMap;
layout (location = 8)  flat in int inUseRoughnessMap;
layout (location = 9)  flat in int inUseAOMap;

layout (location = 10) flat in int inAlbedroMapIndex;
layout (location = 11) flat in int inNormalMapIndex;
layout (location = 12) flat in int inMetallicMapIndex;
layout (location = 13) flat in int inRoughnessMapIndex;
layout (location = 14) flat in int inAOMapIndex;

layout (location = 15) in float inMetallic;
layout (location = 16) in float inRoughness;
layout (location = 17) in vec4 inColor;
layout (location = 18) in mat3 inTBN;

struct Params
{
    vec4 lights;
	vec4 lightColors;
	float exposure;
	float gamma;
};

layout (std140, binding = 12) uniform UBOParams 
{
    Params uboParams;
};

layout (binding = 2) uniform samplerCube samplerIrradiance;
layout (binding = 3) uniform sampler2D samplerBRDFLUT;
layout (binding = 4) uniform samplerCube prefilteredMap;

layout (binding = 24) uniform sampler2D texturesMap[4096];


layout (location = 0) out vec4 outColor;

#define PI 3.1415926535897932384626433832795

vec3 GetAlbedro()
{
	vec3 alb = inUseAlbedroMap == 1 ? texture(texturesMap[inAlbedroMapIndex], inUV).rgb: inColor.rgb;
	return pow(alb, vec3(2.2));
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

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/(PI * denom*denom); 
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
vec3 F_Schlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 prefilteredReflection(vec3 R, float roughness)
{
	const float MAX_REFLECTION_LOD = 9.0; // todo: param/const
	float lod = roughness * MAX_REFLECTION_LOD;
	float lodf = floor(lod);
	float lodc = ceil(lod);
	vec3 a = textureLod(prefilteredMap, R, lodf).rgb;
	vec3 b = textureLod(prefilteredMap, R, lodc).rgb;
	return mix(a, b, lod - lodf);
}

vec3 specularContribution(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0) {
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness); 
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(dotNV, F0);		
		vec3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.001);		
		vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);			
		color += (kD * GetAlbedro() / PI + spec) * dotNL;
	}

	return color;
}

vec3 calculateNormal()
{
    if(inUseNormalMap == 1)
	{
		vec3 tangentNormal = texture(texturesMap[inNormalMapIndex], inUV).xyz * 2.0 - vec3(1.0);
		return inTBN * normalize(tangentNormal);
	}
	else
	{
		return inTBN[2];
	}
}

void main()
{		
	vec3 N = calculateNormal();

	vec3 V = normalize(inCameraPos - inWorldPos);
	vec3 R = reflect(V, N); 

	float metallic = inUseMetallicMap == 1 ? texture(texturesMap[inMetallicMapIndex], inUV).r : inMetallic;
	float roughness = inUseRoughnessMap == 1 ? texture(texturesMap[inRoughnessMapIndex], inUV).r: inRoughness;
	vec3 ao = inUseAOMap == 1 ? texture(texturesMap[inAOMapIndex], inUV).rrr : vec3(1, 1, 1);

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, GetAlbedro(), metallic);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < 1; i++) {
		vec3 L = normalize(uboParams.lights.xyz - inWorldPos);
		Lo += specularContribution(L, V, N, F0, metallic, roughness);
	}   
	
	vec2 brdf = texture(samplerBRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 reflection = prefilteredReflection(R, roughness).rgb;	
	vec3 irradiance = texture(samplerIrradiance, N).rgb;

	// Diffuse based on irradiance
	vec3 diffuse = irradiance * GetAlbedro();	

	vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

	// Specular reflectance
	vec3 specular = reflection * (F * brdf.x + brdf.y);

	// Ambient part
	vec3 kD = 1.0 - F;
	kD *= 1.0 - metallic;	  
	vec3 ambient = (kD * diffuse + specular) * ao;
	
	vec3 color = ambient + Lo;

	// Tone mapping
	color = Uncharted2Tonemap(color * uboParams.exposure);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / uboParams.gamma));

	outColor = vec4(color, 1.0);
}