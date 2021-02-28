#version 450
#define ambient 0.1

// In
layout (location = 0)  in vec3 inWorldPos;
layout (location = 1)  in vec3 inNormal;
layout (location = 2)  in vec3 inCameraPos;
layout (location = 3)  in vec2 inUV;

layout (location = 4)  flat in int inUseAlbedroMap;
layout (location = 5)  flat in int inUseNormalMap;
layout (location = 6)  flat in int inUseMetallicMap;
layout (location = 7)  flat in int inUseRoughnessMap;
layout (location = 8)  flat in int inUseAOMap;

layout (location = 9) flat in int inAlbedroMapIndex;
layout (location = 10) flat in int inNormalMapIndex;
layout (location = 11) flat in int inMetallicMapIndex;
layout (location = 12) flat in int inRoughnessMapIndex;
layout (location = 13) flat in int inAOMapIndex;

layout (location = 14) in float inMetallic;
layout (location = 15) in float inRoughness;
layout (location = 16) in float inExposure;
layout (location = 17) in float inGamma;
layout (location = 18) in float inAmbient;

layout (location = 19) flat in uint inDirectionalLightCount;
layout (location = 20) flat in uint inPointLightCount;

layout (location = 21) in vec4 inColor;
layout (location = 22) in vec4 inShadowCoord;
layout (location = 23) in vec4 inRawPos;
layout (location = 24) in mat3 inTBN;

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
	vec3 outcol = Uncharted2Tonemap(color.rgb * inExposure);
	outcol = outcol * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	return vec4(pow(outcol, vec3(1.0f / inGamma)), color.a);
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

vec4 SRGBtoLINEAR(vec4 srgbIn)
{
	#ifdef MANUAL_SRGB
	#ifdef SRGB_FAST_APPROXIMATION
	vec3 linOut = pow(srgbIn.xyz,vec3(2.2));
	#else //SRGB_FAST_APPROXIMATION
	vec3 bLess = step(vec3(0.04045),srgbIn.xyz);
	vec3 linOut = mix( srgbIn.xyz/vec3(12.92), pow((srgbIn.xyz+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
	#endif //SRGB_FAST_APPROXIMATION
	return vec4(linOut,srgbIn.w);;
	#else //MANUAL_SRGB
	return srgbIn;
	#endif //MANUAL_SRGB
}

vec3 getNormal()
{
    if(inUseNormalMap == 1)
	{
		return inTBN * normalize(texture(texturesMap[inNormalMapIndex], inUV).xyz * 2.0 - vec3(1.0));
	}

	return inTBN[2];
}

void main()
{	
	// Getting Values 
	vec3 N = getNormal();
	vec3 ao = inUseAOMap == 1 ? texture(texturesMap[inAOMapIndex], inUV).rrr : vec3(1, 1, 1);
	vec3 albedro = inUseAlbedroMap == 1 ? texture(texturesMap[inAlbedroMapIndex], inUV).rgb : inColor.rgb;
	albedro = pow(albedro, vec3(2.2));

	float metallic = inUseMetallicMap == 1 ? texture(texturesMap[inMetallicMapIndex], inUV).r : inMetallic;
	float roughness = inUseRoughnessMap == 1 ? texture(texturesMap[inRoughnessMapIndex], inUV).r: inRoughness;
	
	// Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(inCameraPos - inWorldPos);

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
	for(int i=0; i< inDirectionalLightCount; ++i)
	{
		vec3 L = normalize(directionalLights[0].position.xyz - inRawPos.xyz);
		vec3 Lradiance = directionalLights[i].color.rgb;

		// Half-vector between Li and Lo.
		vec3 Lh = normalize(Lo + L);

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
		directLighting += result;
	}

	float shadow = filterPCF(inShadowCoord / inShadowCoord.w);
	directLighting *= shadow;

	// Point lighting calculation
	vec3 pointLighting = vec3(0);
	for(int i=0; i< inPointLightCount; ++i)
	{
		float constantAtt = pointLights[i].params.x;
		float linearAtt = pointLights[i].params.y;
		float expAtt = pointLights[i].params.z;

	    vec3 posToLight = inWorldPos - pointLights[i].position.xyz;
	    float dist = length(posToLight);
	    posToLight = normalize(posToLight);
		float diffuse = max(0.0, dot(N, -posToLight));

		float attTotal = constantAtt + linearAtt * dist + expAtt * dist * dist;
		pointLighting += pointLights[i].color.rgb * (ambientLighting + diffuse) / attTotal;
	}
	vec3 color = directLighting + pointLighting;
	color += ambientLighting;

	// Tone mapping
	color = Uncharted2Tonemap(color * inExposure);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / inGamma));

	outColor = vec4(color, 1.0);
}