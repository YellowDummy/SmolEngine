#version 460 core

layout(location = 0) in vec3 a_Position;

layout(push_constant) uniform DebugData
{
    mat4 model;
	mat4 model2;
	uint state;
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

void main()
{
	switch(state)
	{
		case 0:
        {
	        gl_Position = sceneData.projection * sceneData.view * model * vec4(a_Position, 1);
            break;
        }
        case 1:
        {
			if(gl_VertexIndex == 0)
			{
				gl_Position = sceneData.projection * sceneData.view * model * vec4(a_Position, 1);
			}
			else
			{
				gl_Position = sceneData.projection * sceneData.view * model2 * vec4(a_Position, 1);
			}

            break;
        }
	}
}