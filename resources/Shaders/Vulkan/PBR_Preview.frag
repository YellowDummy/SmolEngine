#version 450 core
layout (location = 0)  in vec3 ModelWorldPos;
layout (location = 1)  in vec3 Normal;
layout (location = 2)  in vec2 UV;
layout (location = 3)  in vec3 WorldPos;
layout (location = 4)  in mat3 TBN;

layout (binding = 2) uniform samplerCube samplerIrradiance;
layout (binding = 3) uniform sampler2D samplerBRDFLUT;
layout (binding = 4) uniform samplerCube prefilteredMap;

layout (binding = 5) uniform sampler2D albedroMap;
layout (binding = 6) uniform sampler2D normalMap;
layout (binding = 7) uniform sampler2D metallicMap;
layout (binding = 8) uniform sampler2D roughnessMap;
layout (binding = 9) uniform sampler2D aoMap;

layout (location = 0) out vec4 outColor;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);
const float PI = 3.141592;
const float Epsilon = 0.00001;

struct StateData
{
    ivec4 states; // x = albedro, y = normal, z = metallic, w = roughness
    ivec4 states_2; // x = ao

    vec4 pbrValues; // x = metallic, y = metallic
    vec4 cameraPos;
};

layout(std140, binding = 100) uniform StateDataBuffer
{   
	StateData data;
};

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

vec3 getNormal()
{
    if(data.states.y == 1)
	{
		return TBN * normalize(texture(normalMap, UV).xyz * 2.0 - vec3(1.0));
	}

	return TBN[2];
}

void main()
{
	// Getting Values 
	vec3 N = getNormal();
	vec3 ao = data.states_2.x == 1 ? texture(aoMap, UV).rrr : vec3(1, 1, 1);
	vec3 albedro = data.states.x == 1 ? texture(albedroMap, UV).rgb : vec3(1.0, 1.0, 1.0);
	albedro = pow(albedro, vec3(2.2));

	float metallic = data.states.z == 1 ? texture(metallicMap, UV).r : data.pbrValues.x;
	float roughness = data.states.w == 1 ? texture(roughnessMap, UV).r: data.pbrValues.y;

    // Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(data.cameraPos.xyz - ModelWorldPos);
    // Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));

	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedro, metallic);
    vec3 ambientLighting;
	{
        float scale = 1.0;
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
		vec3 diffuseIBL = irradiance * ( kd * albedro);

		// Total specular IBL contribution.
		vec3 specularIBL = specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);

		diffuseIBL *= scale; // x = IBL scale
		specularIBL *= scale;

		// Total ambient lighting contribution.
		ambientLighting = diffuseIBL + specularIBL;
	}

    // Direct lighting calculation for analytical lights.
	vec3 directLighting = vec3(0);
	{
        vec3 dir = vec3(0, 63, 15);
		vec3 L = normalize(dir - WorldPos);
		vec3 Lradiance = vec3(1.0);

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
		directLighting += result * 2.0; // temp
	}

    vec3 color =  ambientLighting + directLighting;
    // Tone mapping
	color = Uncharted2Tonemap(color * 4.0);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / 2.5));

	outColor = vec4(color, 1.0);
}