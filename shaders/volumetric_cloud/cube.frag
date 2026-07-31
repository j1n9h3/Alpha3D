#version 330 core

in vec3 worldNormal;
in vec3 localPosition;
in vec3 worldPosition;
noperspective in vec2 screenUV;

out vec4 FragColor;


uniform vec3 cameraPos;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float densityScale;
uniform float extinction;
uniform int maxSteps;
uniform int lightSteps;
uniform sampler3D volumeTexture;

const float PI = 3.14159265359;
const float EPSILON = 0.0001;

uniform mat4 invProjection;
uniform mat4 invView;
uniform mat4 inverseModel;

vec3 GetViewDir() {
    vec2 screenNDC = screenUV * 2.0 - 1.0;
    vec4 viewPos = invProjection * vec4(screenNDC, 1.0, 1.0);
    vec3 viewDir = normalize(viewPos.xyz / viewPos.w);
    vec3 worldDir = normalize((invView * vec4(viewDir, 0.0)).xyz);
    return worldDir;
}

float remap(float value, float inMin, float inMax, float outMin, float outMax)
{
    return outMin + (value - inMin) * (outMax - outMin) /
        max(inMax - inMin, EPSILON);
}

// Equivalent to the pasted cloud() function for the current no-texture prototype.
float cloudDensity(vec3 localPos)
{
    vec3 uvw = localPos + 0.5;
    if (any(lessThan(uvw, vec3(0.0))) || any(greaterThan(uvw, vec3(1.0))))
        return 0.0;

    return texture(volumeTexture, uvw).r * densityScale;
}

float henyeyGreenstein(float g, float mu)
{
    float g2 = g * g;
    return (1.0 - g2) /
        (4.0 * PI * pow(max(1.0 + g2 - 2.0 * g * mu, EPSILON), 1.5));
}

// Returns tNear/tFar for the cube [-0.5, 0.5].
vec2 calculateVolumeIntersection(vec3 rayOrigin, vec3 rayDirection)
{
    vec3 t0 = (-0.5 - rayOrigin) / rayDirection;
    vec3 t1 = ( 0.5 - rayOrigin) / rayDirection;
    vec3 nearValues = min(t0, t1);
    vec3 farValues = max(t0, t1);
    return vec2(max(max(nearValues.x, nearValues.y), nearValues.z),
                min(min(farValues.x, farValues.y), farValues.z));
}

float lightTransmittance(vec3 position, vec3 direction, float stepSize)
{
    float opticalDepth = 0.0;
    for (int i = 0; i < 16; ++i)
    {
        if (i >= lightSteps) break;
        position += direction * stepSize;
        if (any(lessThan(position, vec3(-0.5))) ||
            any(greaterThan(position, vec3(0.5)))) break;
        opticalDepth += cloudDensity(position) * stepSize;
    }
    return exp(-opticalDepth * extinction);
}

void main()
{
    vec3 rayOrigin = (inverseModel * vec4(cameraPos, 1.0)).xyz;
    vec3 worldRayDirection = GetViewDir();
    vec3 rayDirection = normalize((inverseModel * vec4(worldRayDirection, 0.0)).xyz);

    vec2 hit = calculateVolumeIntersection(rayOrigin, rayDirection);
    float t0 = max(hit.x, 0.0);
    float t1 = hit.y;
    if (t1 <= t0) discard;

    float stepSize = (t1 - t0) / float(max(maxSteps, 1));
    vec3 position = rayOrigin + rayDirection * (t0 + stepSize * 0.5);
    float transmittance = 1.0;
    vec3 radiance = vec3(0.0);

    for (int i = 0; i < 96; ++i)
    {
        if (i >= maxSteps || transmittance < 0.01) break;

        float density = cloudDensity(position);
        float sampleT = exp(-density * stepSize * extinction);
        // Self-emission preview: density is written directly as grayscale.
        radiance += transmittance * vec3(density) * stepSize;
        transmittance *= sampleT;

        position += rayDirection * stepSize;
    }

    float alpha = clamp(1.0 - transmittance, 0.0, 1.0);
    FragColor = vec4(radiance, alpha);
}
