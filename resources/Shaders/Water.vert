#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in ivec4 a_BoneIDs;
layout(location = 5) in vec4 a_Weight;

#define NUMBERWAVES 4
const float PI = 3.141592654;
const float G = 9.81;

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

layout (std140, binding = 39) uniform WaterBuffer
{
	float u_passedTime;
    float u_waterPlaneLength;
    vec4 u_waveParameters[NUMBERWAVES];
    vec4 u_waveDirections[NUMBERWAVES];
};

layout(push_constant) uniform ConstantData
{
	mat4 u_inverseViewNormalMatrix;
};

layout(location = 0) out vec3 v_bitangent;
layout(location = 1) out vec3 v_normal;
layout(location = 2) out vec3 v_tangent;
layout(location = 3) out vec2 v_texCoord;
layout(location = 4) out vec3 v_incident;

void main()
{
vec4 finalVertex;

	finalVertex = vec4(a_Position.xyz, 1);

	vec3 finalBitangent;
	vec3 finalNormal;
	vec3 finalTangent;

	finalBitangent.x = 0.0;
	finalBitangent.y = 0.0;
	finalBitangent.z = 0.0;
	
	finalNormal.x = 0.0;
	finalNormal.y = 0.0;
	finalNormal.z = 0.0;

	finalTangent.x = 0.0;
	finalTangent.y = 0.0;
	finalTangent.z = 0.0;

	// see GPU Gems: Chapter 1. Effective Water Simulation from Physical Models
	for (int i = 0; i < NUMBERWAVES; i++)
	{
		vec2 direction = normalize(u_waveDirections[i].xy);
		float speed = u_waveParameters[i].x;
		float amplitude = u_waveParameters[i].y;
		float wavelength = u_waveParameters[i].z;
		float steepness = u_waveParameters[i].w;

		float frequency = sqrt(G*2.0*PI/wavelength);
		float phase = speed*frequency;
		float alpha = frequency*dot(direction, a_Position.xz)+ phase* u_passedTime;
		
		finalVertex.x += steepness*amplitude*direction.x*cos(alpha);
		finalVertex.y += amplitude*sin(alpha);
		finalVertex.z += steepness*amplitude*direction.y*cos(alpha);
	}

	for (int i = 0; i < NUMBERWAVES; i++)
	{
		vec2 direction = normalize(u_waveDirections[i].xy);
		float speed = u_waveParameters[i].x;
		float amplitude = u_waveParameters[i].y;
		float wavelength = u_waveParameters[i].z;
		float steepness = u_waveParameters[i].w;

		float frequency = sqrt(G*2.0*PI/wavelength);
		float phase = speed*frequency;
		float alpha = frequency * dot(direction, finalVertex.xz) + phase*u_passedTime;
				
		// x direction
		finalBitangent.x += steepness * direction.x*direction.x * wavelength * amplitude * sin(alpha);
		finalBitangent.y += direction.x * wavelength * amplitude * cos(alpha);	
		finalBitangent.z += steepness * direction.x*direction.y * wavelength * amplitude * sin(alpha);	
	
		// y direction
		finalNormal.x += direction.x * wavelength * amplitude * cos(alpha);
		finalNormal.y += steepness * wavelength * amplitude * sin(alpha);
		finalNormal.z += direction.y * wavelength * amplitude * cos(alpha);

		// z direction
		finalTangent.x += steepness * direction.x*direction.y * wavelength * amplitude * sin(alpha);
		finalTangent.y += direction.y * wavelength * amplitude * cos(alpha);	
		finalTangent.z += steepness * direction.y*direction.y * wavelength * amplitude * sin(alpha);	
	}

	finalTangent.x = -finalTangent.x;
	finalTangent.z = 1.0 - finalTangent.z;
	finalTangent = normalize(finalTangent);

	finalBitangent.x = 1.0 - finalBitangent.x;
	finalBitangent.z = -finalBitangent.z;
	finalBitangent = normalize(finalBitangent);

	finalNormal.x = -finalNormal.x;
	finalNormal.y = 1.0 - finalNormal.y;
	finalNormal.z = -finalNormal.z;
	finalNormal = normalize(finalNormal);
	
	v_bitangent = finalBitangent;
	v_normal = finalNormal;
	v_tangent = finalTangent;
	
	v_texCoord = vec2(clamp((finalVertex.x+u_waterPlaneLength*0.5-0.5)/u_waterPlaneLength, 0.0, 1.0), clamp((-finalVertex.z+u_waterPlaneLength*0.5+0.5)/u_waterPlaneLength, 0.0, 1.0));

	vec4 vertex = sceneData.view * finalVertex;
	
	// We caculate in world space.
	v_incident = u_inverseViewNormalMatrix * vec3(vertex);	
				
	gl_Position = sceneData.projection * vertex;
}