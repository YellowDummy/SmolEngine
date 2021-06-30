#version 450

layout (binding = 0) uniform sampler2D samplerPositionDepth;
layout (binding = 1) uniform sampler2D samplerNormal;
layout (binding = 2) uniform sampler2D ssaoNoise;

const int SSAO_KERNEL_SIZE = 32;
const float SSAO_RADIUS = 10;

layout (binding = 10) uniform UBOSSAOKernel
{
    vec4 samples[SSAO_KERNEL_SIZE];
};

layout (std140, binding = 27) uniform SceneBuffer
{
	float nearClip;
    float farClip;
    float exoposure;
    float pad;

	mat4 projection;
	mat4 view;
	mat4 skyBoxMatrix;
	vec4 camPos;
	vec4 ambientColor;

} sceneData;

layout (location = 0) in vec2 inUV;
layout (location = 0) out float outFragColor;

void main() 
{
	vec4 normal    = texture(samplerNormal, inUV);
	if(normal.w == 1.0) // uses AO texture
	{
		outFragColor = 0.0;
		return;
	}

	const vec2 noiseScale = vec2(720.0/4.0, 480.0/4.0); // screen = 800x600
    vec4 fragPos   = mat4(0) * vec4(texture(samplerPositionDepth, inUV));
    vec3 randomVec = texture(ssaoNoise, inUV * noiseScale).xyz; 

	vec3 tangent   = normalize(randomVec - normal.xyz * dot(randomVec, normal.xyz));
    vec3 bitangent = cross(normal.xyz, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal.xyz);  

	float occlusion = 0.0;
	const float bias = 0.025f;
    for(int i = 0; i <SSAO_KERNEL_SIZE; ++i)
    {
    // get sample position
    vec3 samplePos = TBN * samples[i].xyz; // from tangent to view-space
    samplePos = fragPos.xyz + samplePos * SSAO_RADIUS;
	vec4 offset = vec4(samplePos, 1.0);
    offset      = sceneData.projection * offset;    // from view to clip-space
    offset.xyz /= offset.w;               // perspective divide
    offset.xyz  = offset.xyz; // transform to range 0.0 - 1.0  
	float sampleDepth = texture(samplerPositionDepth, offset.xy).w; 
	float rangeCheck = smoothstep(0.0, 1.0, SSAO_RADIUS / abs(fragPos.w - sampleDepth));
    occlusion       += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    } 

	occlusion = 1.0 - (occlusion / SSAO_KERNEL_SIZE);
	outFragColor = occlusion; 
}