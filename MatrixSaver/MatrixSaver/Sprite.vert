#version 330 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 texcoord;
layout (location = 2) in mat4 instanceMatrix;


out vec2 TexCoords;
out vec4 Color;
uniform mat4 projection;

void main()
{
	//位置
	vec3 pos = vertex + instanceMatrix[0].xyz;
	pos *= instanceMatrix[0].w;
    gl_Position = projection * vec4(pos, 1.0f);
	//颜色
	Color = instanceMatrix[1];
	TexCoords.x = texcoord.x+0.091f*instanceMatrix[2].x;
	TexCoords.y = texcoord.y+0.167f*instanceMatrix[2].y;
}