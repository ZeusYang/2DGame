#version 330 core
in vec2 TexCoords;
in vec4 Color;
out vec4 color;

uniform sampler2D image;

void main()
{
	vec4 tex = texture(image, TexCoords);
    color.xyz = Color.xyz;
	color.w = (1.0-tex.x)*Color.w;
}