#version 450

layout (binding = 0) uniform sampler2D samplerSkybox;
layout (binding = 1) uniform sampler2D samplerPosition;
layout (binding = 2) uniform sampler2D samplerNormal;
layout (binding = 3) uniform sampler2D samplerAlbedo;
layout (binding = 4) uniform sampler2D samplerPBR;

layout (binding = 5) uniform samplerCube samplerIrradiance;
layout (binding = 6) uniform sampler2D samplerBRDFLUT;
layout (binding = 7) uniform samplerCube prefilteredMap;

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

struct Params
{
    vec4 lights[4];
	vec4 lightColors[4];
    vec4 viewPos;
	float radius;
	int displayMode;
};

layout (std140, binding = 15) uniform UBOParams 
{
    Params ubo;
};

layout (constant_id = 0) const int NUM_SAMPLES = 8;
layout (constant_id = 1) const float MAX_REFLECTION_LOD = 9.0;


#define PI 3.1415926535897932384626433832795
#define ALBEDO pow(texture(samplerAlbedo, inUV).rgb, vec3(2.2))

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
		color += (kD * ALBEDO / PI + spec) * dotNL;
	}

	return color;
}

vec3 calculateLighting(vec3 pos, vec3 normal)
{
	vec3 result = vec3(0.0);

	for(int i = 0; i < ubo.lights.length(); ++i)
	{
		// Vector to light
		vec3 L = ubo.lights[i].xyz - pos;
		// Distance from light to fragment position
		float dist = length(L);

		// Viewer to fragment
		vec3 V = pos;
		V = normalize(V);
		
		// Light to fragment
		L = normalize(L);

		// Attenuation
		float atten = ubo.radius / (pow(dist, 2.0) + 1.0);

		// Diffuse part
		vec3 N = normalize(normal);
		float NdotL = max(0.0, dot(N, L));
		vec3 diff = ubo.lightColors[i].rgb * ALBEDO * NdotL * atten;

		// Specular part
		vec3 R = reflect(-L, N);
		float NdotR = max(0.0, dot(R, V));
		vec3 spec = ubo.lightColors[i].rgb * 1.0 * pow(NdotR, 8.0) * atten;

		result += diff + spec;	
	}

	return result;
}


void main() 
{
	// Getters

	float depth = texture(samplerPosition, inUV).a;
	if(depth == 0)
	{
		outFragcolor = texture(samplerSkybox, inUV);
		return;
	}
	
	vec3 pos = texture(samplerPosition, inUV).rgb;
	vec3 N =  texture(samplerNormal, inUV).rgb;
	vec3 pbrParams = texture(samplerPBR, inUV).rgb;

	float metallic = pbrParams.x;
    float roughness = pbrParams.y;
    vec3 ao = vec3(pbrParams.z , pbrParams.z, pbrParams.z);

	if (ubo.displayMode > 0) {
		switch (ubo.displayMode) {
			case 1: 
				outFragcolor = vec4(pos, 1);
				break;
			case 2: 
				outFragcolor = vec4(N, 1);
				break;
			case 3: 
				outFragcolor = vec4(ALBEDO, 1);
				break;
			case 4: 
				outFragcolor = vec4(texture(samplerAlbedo, inUV).aaa, 1);
				break;
			case 5: 
				outFragcolor = vec4(metallic, roughness, ao.z, 1);
				break;
		}

		return;
	}

	//

	vec3 V = normalize(ubo.viewPos.xyz - pos);
	vec3 R = reflect(-V, N); 

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, ALBEDO, metallic);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < ubo.lights[i].length(); i++) 
	{
		vec3 L = normalize(ubo.lights[i].xyz - pos);
		Lo += specularContribution(L, V, N, F0, metallic, roughness);
	} 

	vec2 brdf = texture(samplerBRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 reflection = prefilteredReflection(R, roughness).rgb;	
	vec3 irradiance = texture(samplerIrradiance, N).rgb;

	// Diffuse based on irradiance
	vec3 diffuse = irradiance * ALBEDO;	

	vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

	// Specular reflectance
	vec3 specular = reflection * (F * brdf.x + brdf.y);

	// Ambient part
	vec3 kD = 1.0 - F;
	kD *= 1.0 - metallic;	  
	vec3 ambient = (kD * diffuse + specular) * ao;
	
	vec3 color = ambient + Lo;

	// Tone mapping
	color = Uncharted2Tonemap(color * 4.0);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / 2.5));

	outFragcolor = vec4(color, 1.0);
}