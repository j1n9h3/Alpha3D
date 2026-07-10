#version 330 core
out vec4 FragColor;

uniform vec3 cameraPos;

in vec2 screenUV;

uniform mat4 invProjection;


vec3 GetViewDir()
{
	vec3 viewDir = vec3(1.0f);
	vec2 screenNDC = screenUV * 2 - 1;  // view range [0, 1] to ndc range [-1, 1]
	vec4 clipPos = vec4(screenNDC, 1.0, 1.0);
	vec4 viewPos = invProjection * clipPos;
	viewPos /= viewPos.w;
	return normalize(viewPos.xyz);
}

void main()
{
	vec3 viewDir = GetViewDir(); // view space view dir
	FragColor = vec4(vec3((viewDir * 0.5 + 0.5).y), 1.0);
}