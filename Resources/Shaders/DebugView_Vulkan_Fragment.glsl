#version 450

layout (location = 0) in vec2 inUV;

layout (binding = 0) uniform sampler2DArray shadowMap;

layout(push_constant) uniform InstanceData
{
	uint  showCascades;
	uint  showMRT;

    uint  mrtAttachmentIndex;
    uint  cascadeIndex;
};

layout (location = 0) out vec4 FragColor;

void main() 
{
    float depth = texture(shadowMap, vec3(inUV, float(cascadeIndex))).r;
	FragColor = vec4(vec3((depth)), 1.0);
}