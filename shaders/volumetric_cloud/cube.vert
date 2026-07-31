#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 worldNormal;
out vec3 localPosition;
out vec3 worldPosition;

noperspective out vec2 screenUV;

void main()
{
    localPosition = aPos;
    worldPosition = vec3(model * vec4(aPos, 1.0));
    worldNormal = normalize(
        mat3(transpose(inverse(model))) * aNormal
    );

    vec4 clipPosition =
        projection * view * vec4(worldPosition, 1.0);

    gl_Position = clipPosition;

    screenUV = clipPosition.xy / clipPosition.w;
    screenUV = screenUV * 0.5 + 0.5;
}