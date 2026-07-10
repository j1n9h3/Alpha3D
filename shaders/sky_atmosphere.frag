#version 330 core

out vec4 FragColor;

in vec2 screenUV;

uniform vec3 cameraPos;
uniform mat4 invProjection;

uniform vec3 lightDirection;
uniform float lightIntensity;
uniform float cameraHeight;
uniform float planetRadius;
uniform float atmosphereRadius;
uniform vec3 rayleighBeta;
uniform vec3 mieBeta;
uniform vec3 absorptionBeta;
uniform float rayleighHeight;
uniform float mieHeight;
uniform float absorptionHeight;
uniform float absorptionFalloff;
uniform float mieG;
uniform float exposure;
uniform float gamma;
const int PRIMARY_STEPS = 32;
const int LIGHT_STEPS = 8;

vec3 GetViewDir()
{
    vec2 screenNDC = screenUV * 2.0 - 1.0;
    vec4 viewPos = invProjection * vec4(screenNDC, 1.0, 1.0);
    return normalize(viewPos.xyz / viewPos.w);
}

vec2 RaySphereIntersect(vec3 origin, vec3 direction, float radius)
{
    float b = 2.0 * dot(direction, origin);
    float c = dot(origin, origin) - radius * radius;
    float discriminant = b * b - 4.0 * c;
    if (discriminant < 0.0)
        return vec2(1e20, -1e20);

    float root = sqrt(discriminant);
    return vec2((-b - root) * 0.5, (-b + root) * 0.5);
}

vec3 AtmosphereDensity(float height)
{
    vec2 particleDensity = exp(-height / vec2(rayleighHeight, mieHeight));
    float ozoneOffset = (absorptionHeight - height) / absorptionFalloff;
    float ozoneDensity = particleDensity.x / (ozoneOffset * ozoneOffset + 1.0);
    return vec3(particleDensity, ozoneDensity);
}

vec3 CalculateScattering(vec3 rayOrigin, vec3 rayDirection, float maxDistance,
                         vec3 background, vec3 lightDirection)
{
    vec2 atmosphereHit = RaySphereIntersect(rayOrigin, rayDirection, atmosphereRadius);
    if (atmosphereHit.x > atmosphereHit.y)
        return background;

    float rayStart = max(atmosphereHit.x, 0.0);
    float rayEnd = min(atmosphereHit.y, maxDistance);
    if (rayStart >= rayEnd)
        return background;

    bool allowMie = maxDistance > atmosphereHit.y;
    float stepSize = (rayEnd - rayStart) / float(PRIMARY_STEPS);
    float rayPosition = rayStart + stepSize * 0.5;

    vec3 opticalDepth = vec3(0.0);
    vec3 totalRayleigh = vec3(0.0);
    vec3 totalMie = vec3(0.0);

    float mu = dot(rayDirection, lightDirection);
    float mu2 = mu * mu;
    float g2 = mieG * mieG;
    float rayleighPhase = 3.0 * (1.0 + mu2) / (16.0 * 3.14159265359);
    float miePhase = 3.0 * (1.0 - g2) * (1.0 + mu2) /
                     (8.0 * 3.14159265359 * (2.0 + g2) *
                      pow(max(1.0 + g2 - 2.0 * mieG * mu, 1e-4), 1.5));
    if (!allowMie)
        miePhase = 0.0;

    for (int i = 0; i < PRIMARY_STEPS; ++i)
    {
        vec3 samplePosition = rayOrigin + rayDirection * rayPosition;
        float height = max(length(samplePosition) - planetRadius, 0.0);
        vec3 localDensity = AtmosphereDensity(height) * stepSize;
        opticalDepth += localDensity;

        vec2 lightHit = RaySphereIntersect(samplePosition, lightDirection, atmosphereRadius);
        float lightStepSize = max(lightHit.y, 0.0) / float(LIGHT_STEPS);
        float lightPosition = lightStepSize * 0.5;
        vec3 lightOpticalDepth = vec3(0.0);

        for (int j = 0; j < LIGHT_STEPS; ++j)
        {
            vec3 lightSample = samplePosition + lightDirection * lightPosition;
            float lightHeight = max(length(lightSample) - planetRadius, 0.0);
            lightOpticalDepth += AtmosphereDensity(lightHeight) * lightStepSize;
            lightPosition += lightStepSize;
        }

        vec3 combinedDepth = opticalDepth + lightOpticalDepth;
        vec3 attenuation = exp(-rayleighBeta * combinedDepth.x
                               - mieBeta * combinedDepth.y
                               - absorptionBeta * combinedDepth.z);
        totalRayleigh += localDensity.x * attenuation;
        totalMie += localDensity.y * attenuation;
        rayPosition += stepSize;
    }

    vec3 transmittance = exp(-rayleighBeta * opticalDepth.x
                             - mieBeta * opticalDepth.y
                             - absorptionBeta * opticalDepth.z);
    vec3 scatteredLight = (rayleighPhase * rayleighBeta * totalRayleigh
                         + miePhase * mieBeta * totalMie) * lightIntensity;
    return scatteredLight + background * transmittance;
}

void main()
{
    vec3 viewDirection = GetViewDir();

    // The renderer currently supplies a view-space ray only. Keep the atmosphere
    // centred on the camera horizontally and use cameraPos to preserve small
    // vertical camera movements without losing floating-point precision.
    float heightAboveGround = max(cameraPos.y, 0.0) + cameraHeight;
    vec3 atmosphereCamera = vec3(0.0, planetRadius + heightAboveGround, 0.0);
    vec3 sunDirection = normalize(lightDirection);

    vec2 planetHit = RaySphereIntersect(atmosphereCamera, viewDirection, planetRadius);
    float sceneDepth = 1e20;
    vec3 sceneColor = vec3(0.0);

    if (planetHit.y > 0.0)
    {
        sceneDepth = max(planetHit.x, 0.0);
        vec3 surfacePosition = atmosphereCamera + viewDirection * sceneDepth;
        vec3 normal = normalize(surfacePosition);
        float diffuse = max(dot(normal, sunDirection), 0.0);
        sceneColor = vec3(0.008, 0.012, 0.006) * (0.08 + 0.25 * diffuse);
    }
    else if (dot(viewDirection, sunDirection) > 0.9998)
    {
        sceneColor = vec3(3.0);
    }

    vec3 color = CalculateScattering(atmosphereCamera, viewDirection, sceneDepth,
                                     sceneColor, sunDirection);
    color = 1.0 - exp(-color * exposure);
    color = pow(max(color, vec3(0.0)), vec3(1.0 / gamma));
    FragColor = vec4(color, 1.0);
}
