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
layout (location = 17) in float inExposure;
layout (location = 18) in float inGamma;
layout (location = 19) in float inAmbient;

layout (location = 20) flat in int inDirectionalLightCount;

layout (location = 21) in vec4 inColor;
layout (location = 22) in mat3 inTBN;

layout (binding = 2) uniform samplerCube samplerIrradiance;
layout (binding = 3) uniform sampler2D samplerBRDFLUT;
layout (binding = 4) uniform samplerCube prefilteredMap;

layout (binding = 24) uniform sampler2D texturesMap[4096];

struct DirectionalLightBuffer
{
    vec4 position;
	vec4 color;
};

layout(std140, binding = 28) readonly buffer DirectionalLightStorage
{   
	DirectionalLightBuffer directionalLights[];
};

layout (location = 0) out vec4 outColor;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);
const float PI = 3.141592;
const float Epsilon = 0.00001;

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

	float metallic = inUseMetallicMap == 1 ? texture(texturesMap[inMetallicMapIndex], inUV).r : inMetallic;
	float roughness = inUseRoughnessMap == 1 ? texture(texturesMap[inRoughnessMapIndex], inUV).r: inRoughness;
	vec3 ao = inUseAOMap == 1 ? texture(texturesMap[inAOMapIndex], inUV).rrr : vec3(1, 1, 1);
	// Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(inCameraPos - inWorldPos);

	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));

	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, GetAlbedro(), metallic);

	// Direct lighting calculation for analytical lights.
	vec3 directLighting = vec3(0);
	for(int i=0; i< inDirectionalLightCount; ++i)
	{
		vec3 Li = normalize(directionalLights[i].position.xyz);
		vec3 Lradiance = directionalLights[i].color.rgb;

		// Half-vector between Li and Lo.
		vec3 Lh = normalize(Li + Lo);

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(N, Li));
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
		vec3 diffuseBRDF = kd * GetAlbedro();

		// Cook-Torrance specular microfacet BRDF.
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		// Total contribution for this light.
		directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	}

    // Ambient lighting (IBL).
	vec3 ambientLighting;
	{
		// Sample diffuse irradiance at normal direction.
		vec3 irradiance = texture(samplerIrradiance, N).rgb;

		// Calculate Fresnel term for ambient lighting.
		// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
		// use cosLo instead of angle with light's half-vector (cosLh above).
		// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
		vec3 F = fresnelSchlick(F0, cosLo);

		// Get diffuse contribution factor (as with direct lighting).
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

		// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
		vec3 diffuseIBL = kd * GetAlbedro() * irradiance;

		// Sample pre-filtered specular reflection environment at correct mipmap level.
		int specularTextureLevels = textureQueryLevels(prefilteredMap);
		vec3 specularIrradiance = textureLod(prefilteredMap, Lr, roughness * specularTextureLevels).rgb;

		// Split-sum approximation factors for Cook-Torrance specular BRDF.
		vec2 specularBRDF = texture(samplerBRDFLUT, vec2(cosLo, roughness)).rg;

		// Total specular IBL contribution.
		vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

		// Total ambient lighting contribution.
		ambientLighting = mix(diffuseIBL, specularIBL, 0.04);
	}
	
	vec3 color = directLighting + ambientLighting;

	// Tone mapping
	color = Uncharted2Tonemap(color * inExposure);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / inGamma));

	outColor = vec4(color, 1.0);
}