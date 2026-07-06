#version 330 core
out vec4 FragColor;

uniform vec3 viewPos;


void main()
{
	FragColor = vec4(viewPos, 1.0);
}