#version 330 core

in vec2 screenUV;
out vec4 FragColor;

uniform float planetRadius;
uniform float atmosphereRadius;
uniform bool useRayleigh;
uniform bool useMie;
uniform bool useAbsorption;
uniform vec3 rayleighBeta;
uniform vec3 mieBeta;
uniform vec3 absorptionBeta;
uniform float rayleighHeight;
uniform float mieHeight;
uniform float absorptionHeight;
uniform float absorptionFalloff;
uniform int transmittanceSteps;

// Bruneton-style parameterization. It maps the LUT without wasting texels on
// rays that immediately hit the planet.
void uvToTransmittanceParams(vec2 uv, out float viewHeight, out float viewZenithCos)
{
    float H = sqrt(max(atmosphereRadius * atmosphereRadius
                       - planetRadius * planetRadius, 0.0));
    float rho = H * clamp(uv.y, 0.0, 1.0);
    viewHeight = sqrt(rho * rho + planetRadius * planetRadius);

    float dMin = atmosphereRadius - viewHeight;
    float dMax = rho + H;
    float d = mix(dMin, dMax, clamp(uv.x, 0.0, 1.0));
    if (d <= 1e-6) {
        viewZenithCos = 1.0;
        return;
    }

    viewZenithCos = (H * H - rho * rho - d * d)
                    / (2.0 * viewHeight * d);
    viewZenithCos = clamp(viewZenithCos, -1.0, 1.0);
}

float distanceToAtmosphereTop(float radius, float mu)
{
    float discriminant = radius * radius * (mu * mu - 1.0)
                         + atmosphereRadius * atmosphereRadius;
    return max(-radius * mu + sqrt(max(discriminant, 0.0)), 0.0);
}

vec3 AtmosphereDensity(float height)
{
    height = max(height, 0.0);
    vec2 particleDensity = exp(-height / vec2(rayleighHeight, mieHeight));
    float ozoneOffset = (absorptionHeight - height) / absorptionFalloff;
    float ozoneDensity = particleDensity.x / (ozoneOffset * ozoneOffset + 1.0);

    return vec3(useRayleigh ? particleDensity.x : 0.0,
                useMie ? particleDensity.y : 0.0,
                useAbsorption ? ozoneDensity : 0.0);
}

vec3 computeTransmittance(float viewHeight, float viewZenithCos)
{
    float rayLength = distanceToAtmosphereTop(viewHeight, viewZenithCos);
    int sampleCount = max(transmittanceSteps, 1);
    float stepLength = rayLength / float(sampleCount);
    vec3 opticalDepth = vec3(0.0);

    // The midpoint rule avoids sampling exactly on either spherical boundary.
    for (int i = 0; i < sampleCount; ++i) {
        float distanceAlongRay = (float(i) + 0.5) * stepLength;
        float sampleRadius = sqrt(max(
            viewHeight * viewHeight
            + distanceAlongRay * distanceAlongRay
            + 2.0 * viewHeight * viewZenithCos * distanceAlongRay,
            planetRadius * planetRadius));
        opticalDepth += AtmosphereDensity(sampleRadius - planetRadius) * stepLength;
    }

    vec3 extinction = rayleighBeta * opticalDepth.x
                    + mieBeta * opticalDepth.y
                    + absorptionBeta * opticalDepth.z;
    return exp(-max(extinction, vec3(0.0)));
}

void main()
{
    float viewHeight;
    float viewZenithCos;
    uvToTransmittanceParams(clamp(screenUV, vec2(0.0), vec2(1.0)),
                            viewHeight, viewZenithCos);
    FragColor = vec4(computeTransmittance(viewHeight, viewZenithCos), 1.0);
}
