#version 330 core

out vec4 FragColor;

in vec2 screenUV;

uniform vec3 cameraPos;
uniform mat4 invProjection;
uniform mat4 invView;

uniform bool useRayleigh;
uniform bool useMie;
uniform bool useAbsorption;

uniform bool useTransmittanceLUT;

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

uniform int primarySteps;
uniform int lightSteps;
uniform int transmittanceLUTSteps;

uniform int MODE;
// 0 Single Scattering
// 1 Compute Transmitance LUT
// 2 Compute Sky-view LUT
// 3 Compute xxx LUT
// 4 Compute Multi-scattering LUT

uniform sampler2D transmittanceLUT;


vec3 GetViewDir()
{
    vec2 screenNDC = screenUV * 2.0 - 1.0;
    vec4 viewPos = invProjection * vec4(screenNDC, 1.0, 1.0);
    vec3 viewDir = normalize(viewPos.xyz / viewPos.w);
    vec3 worldDir = normalize((invView * vec4(viewDir, 0.0)).xyz);
    return worldDir;
}


vec3 AtmosphereDensity(float height)
{
    vec2 particleDensity = exp(-height / vec2(rayleighHeight, mieHeight));
    float ozoneOffset = (absorptionHeight - height) / absorptionFalloff;
    float ozoneDensity = particleDensity.x / (ozoneOffset * ozoneOffset + 1.0);
    return vec3(useRayleigh ? particleDensity.x : 0.0,
                useMie ? particleDensity.y : 0.0,
                useAbsorption ? ozoneDensity : 0.0);
}


vec2 RaySphereIntersect(vec3 origin, vec3 direction, float radius)
{
    float b = 2.0 * dot(direction, origin);
    float c = dot(origin, origin) - radius * radius;
    float discriminant = b * b - 4.0 * c;
    if (discriminant < 0.0) return vec2(1e20, -1e20);
    float root = sqrt(discriminant);
    return vec2((-b - root) * 0.5, (-b + root) * 0.5);
}

void uvToTransmittanceParams(vec2 uv, out float viewHeight, out float viewZenithCos)
{
    float H = sqrt(max(atmosphereRadius * atmosphereRadius - planetRadius * planetRadius, 0.0));
    float rho = H * clamp(uv.y, 0.0, 1.0);
    viewHeight = sqrt(rho * rho + planetRadius * planetRadius);
    float dMin = atmosphereRadius - viewHeight;
    float dMax = rho + H;
    float d = mix(dMin, dMax, clamp(uv.x, 0.0, 1.0));
    if (d <= 1e-6) { viewZenithCos = 1.0; return; }
    viewZenithCos = (H * H - rho * rho - d * d) / (2.0 * viewHeight * d);
    viewZenithCos = clamp(viewZenithCos, -1.0, 1.0);
}



vec2 transmittanceParamsToUv(float viewRadius, float viewZenithCos)
{
    float H = sqrt(max(atmosphereRadius * atmosphereRadius - planetRadius * planetRadius, 0.0));
    float rho = sqrt(max(viewRadius * viewRadius - planetRadius * planetRadius, 0.0));
    float discriminant = viewRadius * viewRadius * (viewZenithCos * viewZenithCos - 1.0) + atmosphereRadius * atmosphereRadius;
    float distanceToTop = -viewRadius * viewZenithCos + sqrt(max(discriminant, 0.0));
    float dMin = atmosphereRadius - viewRadius;
    float dMax = rho + H;
    float xMu = (distanceToTop - dMin) / max(dMax - dMin, 1e-6);
    float xR = rho / max(H, 1e-6);
    return clamp(vec2(xMu, xR), vec2(0.0), vec2(1.0));
}


vec3 computeTransmittance(vec3 position, vec3 direction, int sampleCount)
{
    vec3 upDirection = normalize(position);
    float viewHeight = length(position);
    float viewZenithCos = dot(direction, upDirection);
    float rayLength = RaySphereIntersect(position, direction, atmosphereRadius)[1];
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



vec3 SampleTransmittanceLUT(vec3 position, vec3 direction)
{
    float radius = length(position);
    vec3 upDirection = position / radius;
    float zenithCos = dot(upDirection, direction);
    vec2 uv = transmittanceParamsToUv(radius, zenithCos);
    return texture(transmittanceLUT, uv).rgb;
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
    int viewSampleCount = max(primarySteps, 1);
    int lightSampleCount = max(lightSteps, 1);
    float stepSize = (rayEnd - rayStart) / float(viewSampleCount);
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

    for (int i = 0; i < viewSampleCount; ++i)
    {
        vec3 samplePosition = rayOrigin + rayDirection * rayPosition;
        float height = max(length(samplePosition) - planetRadius, 0.0);
        vec3 localDensity = AtmosphereDensity(height) * stepSize;
        opticalDepth += localDensity;

        vec2 lightHit = RaySphereIntersect(samplePosition, lightDirection, atmosphereRadius);

        vec3 lightOpticalDepth = vec3(0.0);
        vec3 attenuation = vec3(0.0);

        vec3 viewTransmittance = exp(-rayleighBeta * opticalDepth.x - mieBeta * opticalDepth.y - absorptionBeta * opticalDepth.z);

        if (useTransmittanceLUT) {
            attenuation = viewTransmittance * SampleTransmittanceLUT(samplePosition, lightDirection);;
        }
        else {
            attenuation = viewTransmittance * computeTransmittance(samplePosition, lightDirection, lightSampleCount);
        }

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

vec4 SingleScattering(){

    vec3 viewDirection = GetViewDir();
    float heightAboveGround = max(cameraPos.y, 0.0) + cameraHeight;
    vec3 atmosphereCamera = vec3(cameraPos.x, planetRadius + cameraHeight + cameraPos.y, cameraPos.z);
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

    vec3 color = CalculateScattering(atmosphereCamera, viewDirection, sceneDepth, sceneColor, sunDirection);
    color = 1.0 - exp(-color * exposure);
    color = pow(max(color, vec3(0.0)), vec3(1.0 / gamma));
    return vec4(color, 1.0);
}

vec3 zenithCosToDirection(float cosZenith, float azimuth)
{
    float sinZenith = sqrt(max(0.0, 1.0 - cosZenith * cosZenith));

    return vec3(sinZenith * cos(azimuth), cosZenith, sinZenith * sin(azimuth));
}

vec4 computeTransmittanceLUT(){
    int sampleCount = max(transmittanceLUTSteps, 1);
    float viewHeight;
    float viewZenithCos;
    uvToTransmittanceParams(clamp(screenUV, vec2(0.0), vec2(1.0)), viewHeight, viewZenithCos);
    vec3 assumptionPosition = vec3(0, viewHeight, 0);
    vec3 assumptionDirection = zenithCosToDirection(viewZenithCos, 0);
    return vec4(computeTransmittance(assumptionPosition, assumptionDirection, sampleCount), 1.0);
}

void main()
{
    switch(MODE){
        case 0:
            FragColor = SingleScattering();
            break;
        case 1:
            FragColor = computeTransmittanceLUT();
            break;            
    }
}
