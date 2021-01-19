#version 450 core

layout(location = 0) out vec4 o_color;

layout(location = 4) in vec4 v_Color;
layout(location = 5) in vec2 v_UV;
layout(location = 6) in vec3 v_Normal;
layout(location = 7) in vec3 v_WorldPos;
layout(location = 8) in vec3 v_Camera;

layout(binding = 9) uniform sampler2D u_Textures[7];

struct Light2DBuffer
{
	vec4 LightColor;
	vec4 Position;
	vec4 Attributes; // r = radius, g = intensity
};

layout(std140, binding = 10) uniform LightBuffer
{
	Light2DBuffer LightData[2];
};


const float PI = 3.14159265359;
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main()
{

	float ao = texture(u_Textures[0], v_UV).r;
	float roughness = texture(u_Textures[3], v_UV).r;
	float metallic = texture(u_Textures[1], v_UV).r;;

	vec3 albedo = pow(texture(u_Textures[0], v_UV).rgb, vec3(2.2));

    vec4 result = texture(u_Textures[0], v_UV);
	for(int i = 1; i < 7; ++i)
	{
		result *= texture(u_Textures[i], v_UV);
	}
	vec4 finalColor = result * v_Color;

	vec3 N = normalize(v_Normal);
    vec3 V = normalize(v_Camera - v_WorldPos);
	vec3 F0 = vec3(0.06); 
    F0 = mix(F0, albedo, metallic);

	// reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 2; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(LightData[i].Position.rgb - v_WorldPos);
        vec3 H = normalize(V + L);
        float distance    = length(LightData[i].Position.rgb - v_WorldPos);
        float attenuation =LightData[i].Attributes.r / (distance * distance);
        vec3 radiance     = LightData[i].LightColor.rgb * attenuation;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }

	vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

	o_color = vec4(color, 1.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}