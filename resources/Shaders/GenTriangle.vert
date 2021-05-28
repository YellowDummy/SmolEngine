#version 450

layout (location = 0) out vec2 texcoord;

vec2 flipTexCoord(vec2 tc) {
    return tc * vec2(1.0, -1.0) + vec2(0.0, 1.0);
}

void main()
{
    texcoord.x = (gl_VertexIndex == 2) ?  2.0 :  0.0;
    texcoord.y = (gl_VertexIndex == 1) ?  2.0 :  0.0;
	vec2 pos =  texcoord * vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    gl_Position = vec4(pos, 1.0, 1.0);
}
