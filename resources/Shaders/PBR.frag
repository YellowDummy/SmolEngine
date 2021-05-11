#version 460

// In
layout (location = 0)  in vec3 v_FragPos;
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

layout (location = 16) in vec4 v_Color;
layout (location = 17) in vec4 v_ShadowCoord;
layout (location = 18) in vec4 v_WorldPos;
layout (location = 19) in mat3 v_TBN;

// Out
layout (location = 0) out vec4 outColor0;
layout (location = 1) out vec4 outColor1;

// Samplers
layout (binding = 1) uniform sampler2D shadowMap;
layout (binding = 2) uniform samplerCube samplerIrradiance;
layout (binding = 3) uniform sampler2D samplerBRDFLUT;
layout (binding = 4) uniform samplerCube prefilteredMap;
layout (binding = 24) uniform sampler2D texturesMap[4096];

// Buffers
// -----------------------------------------------------------------------------------------------------------------------
struct SpotLight 
{
    vec4 position;
	vec4 direction;
	vec4 color;
	
	float intensity;
	float cutOff;
	float outerCutOff;
    float raduis;
	
	float bias;
	uint is_active;
	uint cast_shadows;
};

struct PointLight 
{
    vec4 position;
    vec4 color;
	
	float intensity;
    float raduis;
	float bias;
	uint is_active;
	uint cast_shadows;
};

layout(std140, binding = 30) readonly buffer PointLightBuffer
{   
	PointLight pointLights[];
};

layout(std140, binding = 31) readonly buffer SpotLightBuffer
{   
	SpotLight spotLights[];
};

layout(std140, binding = 32) uniform DirLightBuffer
{   
	vec4 direction;
	vec4 color;
	float intensity;
	float bias;
	uint is_active;
	uint cast_shadows;
	uint soft_shadows;
} dirLight;

layout(std140, binding = 33) uniform SceneState
{   
	float hdrExposure;
	uint use_ibl;
	uint numPointsLights;
	uint numSpotLights;

} sceneState;

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

float insideBox3d(vec3 v, vec3 bl, vec3 br)
{
	vec3 s = step(bl, v) - step(br, v);
	return s.x * s.y * s.z;
}

float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( shadowMap, shadowCoord.st + off ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z ) 
		{
			shadow = 0.1;
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
	specularIBL = specularIBL / 2.5;
    
	return (diffuseIBL + specularIBL) * ao;
}

vec3 CalcDirLight(vec3 V, vec3 N, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{
	vec3 L = normalize(dirLight.direction.xyz - v_FragPos);
    vec3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0);       
	
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness_color);   
    float G   = GeometrySmith(N, V, L, roughness_color);      
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
	
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic_color);
	vec3 diffuseBRDF = kd * albedo_color;
	vec3 specularBRDF = (F * NDF * G) / max(Epsilon, 4.0 * NdotL * 0.8);
	
	return ((albedo_color / PI + diffuseBRDF + specularBRDF) * vec3(1.0) * NdotL) * dirLight.intensity * 5.0 * dirLight.color.rgb;
}

vec3 CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{           
    float distance = length(light.position.xyz - v_FragPos);
	if(distance > light.raduis) return vec3(0);

	vec3 L = normalize(light.position.xyz - v_FragPos);
    vec3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0);       
	
	//float attenuation = 1 * ((distance + light.quadratic/light.linear) * (distance * distance)) / (light.linear * distance);

    float attenuation;// = smoothstep(light.raduis, 0, distance);//1.0 - (distance / (light.raduis));
	attenuation = clamp(1.0 - distance/light.raduis, 0.0, 1.0); attenuation *= attenuation;
	//attenuation = clamp(1.0 - distance*distance/(light.raduis*light.raduis), 0.0, 1.0); attenuation *= attenuation;
	//if(attenuation < 0) attenuation = 0;
	//if(attenuation > 1) attenuation = 1;
	
    vec3 radiance = vec3(1.0) * attenuation * light.intensity;
	
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness_color);   
    float G   = GeometrySmith(N, V, L, roughness_color);      
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
	
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic_color);
	vec3 diffuseBRDF = kd * albedo_color;
	vec3 specularBRDF = (F * NDF * G) / max(Epsilon, 4.0 * NdotL * 0.8);      
	
	return (albedo_color / PI + diffuseBRDF + specularBRDF) * radiance * NdotL * light.color.rgb;
}

vec3 CalcSpotLight(SpotLight light, vec3 V, vec3 N, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{
    float distance = length(light.position.xyz - v_FragPos);
	if(distance > light.raduis) return vec3(0);
	
	vec3 L = normalize(light.position.xyz - v_FragPos);
	float theta = dot(L, normalize(-light.direction.xyz));
	
	if(theta > light.outerCutOff) return vec3(0); // we're working with angles as cosines instead of degrees so a '>' is used.
	
	// spotlight (soft edges)
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensitys = clamp((light.outerCutOff - theta) / epsilon, 0.0, 1.0);
	vec3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0);   

    float attenuation = 1.0 - (distance / (light.raduis));
	if(attenuation < 0) attenuation = 0;
	if(attenuation > 1) attenuation = 1;
	
    vec3 radiance = vec3(1.0) * attenuation * light.intensity * intensitys;
	
    // Cook-Torrance BRDF direction
    float NDF = DistributionGGX(N, H, roughness_color);   
    float G   = GeometrySmith(N, V, L, roughness_color);      
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
	
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic_color);
	vec3 diffuseBRDF = kd * albedo_color;
	vec3 specularBRDF = (F * NDF * G) / max(Epsilon, 4.0 * NdotL * 0.8);	       
	
	return (albedo_color / PI + diffuseBRDF + specularBRDF) * radiance * NdotL * light.color.rgb;
}

vec3 getNormal()
{
	if(v_UseNormalMap == 1)
	{
		 vec3 tangentNormal = texture(texturesMap[v_NormalMapIndex], v_UV).xyz * 2.0 - 1.0;
	    return normalize(v_TBN * tangentNormal);
	}
	
	return normalize(v_Normal);
}

void main()
{	
	// Getting Values
	//--------------------------------------------
	vec3 N = getNormal(); 						
	vec3 V = normalize(v_CameraPos - v_FragPos); 
	vec3 ao = v_UseAOMap == 1 ? texture(texturesMap[v_AOMapIndex], v_UV).rrr : vec3(1, 1, 1);
	vec3 albedro = v_UseAlbedroMap == 1 ? texture(texturesMap[v_AlbedroMapIndex], v_UV).rgb : v_Color.rgb;
	float metallic = v_UseMetallicMap == 1 ? texture(texturesMap[v_MetallicMapIndex], v_UV).r : v_Metallic;
	float roughness = v_UseRoughnessMap == 1 ? texture(texturesMap[v_RoughnessMapIndex], v_UV).r: v_Roughness;
	albedro = pow(albedro, vec3(2.2));

	// Ambient Lighting (IBL)
	//--------------------------------------------
	vec3 F0 = mix(vec3(0.04), albedro, metallic); 
    vec3 ambient = vec3(0.0);
	if(sceneState.use_ibl == 1)
	{
		ambient = CalcIBL(N, V, F0, ao, albedro, metallic, roughness);
	}

	vec3 Lo = vec3(0.0);
	// Direct Lighting
	//--------------------------------------------
	 if(dirLight.is_active == 1)
	 {
		 Lo += CalcDirLight(V, N, F0, albedro, metallic, roughness);
	 }

	// Point Lighting
	//--------------------------------------------
	for(int i = 0; i < sceneState.numPointsLights; i++)
	{
		if(pointLights[i].is_active == 1)
		{
			Lo += CalcPointLight(pointLights[i], V, N, F0, albedro, metallic, roughness);
		}
	}

	// Spot Lighting
	//--------------------------------------------
	for(int i = 0; i < sceneState.numSpotLights; i++)
	{
		if(spotLights[i].is_active == 1)
		{
			Lo += CalcSpotLight(spotLights[i], V, N, F0, albedro, metallic, roughness);
		}
	}

	// Final Shading
	//--------------------------------------------
	vec3 color = ambient + Lo;
	float shadow = 0.0;
	if(dirLight.cast_shadows == 1)
	{
		if(dirLight.soft_shadows == 1)
		{
			shadow = filterPCF(v_ShadowCoord / v_ShadowCoord.w);
		}
		else
		{
			ivec2 texDim = textureSize(shadowMap, 0);
			shadow = textureProj(v_ShadowCoord, texDim);
		}

		color *= shadow;
    }

	// Emission
	//--------------------------------------------

	//if (use_emission)
	//{
		// emission
	//	vec3 emi = emission * emission_power;
	//	if(use_tex_emission){
	//		emi *= texture(tex_emission, TexCoords).r;
	//	}
	//	color += emi;
	//}

	// HDR
	//--------------------------------------------
	// Color with manual exposure into attachment 0

	outColor0.rgb = vec3(1.0) - exp(-color.rgb * sceneState.hdrExposure);

	// Bright parts for bloom into attachment 1
	float l = dot(outColor0.rgb, vec3(0.2126, 0.7152, 0.0722));
	float threshold = 0.75;
	outColor1.rgb = (l > threshold) ? outColor0.rgb : vec3(0.0);
	outColor1.a = 1.0;
}