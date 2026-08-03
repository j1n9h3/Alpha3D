#version 330 core

in vec3 worldNormal;
in vec3 localPosition;
in vec3 worldPosition;
noperspective in vec2 screenUV;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 ambientLight;
uniform float lightIntensity;
uniform float densityScale;
uniform float extinction;
uniform float shapeScale;
uniform float detailScale;
uniform float erosionStrength;
uniform vec3 windDirection;
uniform float cloudSpeed;
uniform float cloudTopOffset;
uniform float anvilBias;
uniform float cloudCoverageBlend;
uniform float time;
uniform float cubeNoiseScale;
uniform float cubeDetailStrength;
uniform float cubeDensityThreshold;
uniform float cubeEdgeSoftness;
uniform float cubeBottomFade;
uniform float cubeTopFade;
uniform int maxSteps;
uniform int lightSteps;
uniform float rayJitterStrength;
uniform float transmittanceCutoff;
uniform float phaseG;
uniform int MODE;

const float PI = 3.14159265359;
const float EPSILON = 0.0001;

uniform mat4 invProjection;
uniform mat4 invView;
uniform mat4 model;
uniform mat4 inverseModel;

uniform sampler3D perlinWorleyNoiseTexture;
uniform sampler3D worleyNoise1;
uniform sampler3D worleyNoise2;
uniform sampler3D worleyNoise3;
uniform sampler3D highFrequencyNoiseTexture;
// R/G: cloud coverage probabilities, B: cloud type probability.
uniform sampler2D cloudMapTexture;
uniform sampler2D cumulusHeightTexture;
uniform sampler2D cumulonimbusHeightTexture;
uniform sampler2D stratusHeightTexture;
uniform sampler2D baseReduceTexture;

float remap(float value, float inMin, float inMax, float outMin, float outMax);

float Remap(float value, float inMin, float inMax, float outMin, float outMax) { return remap(value, inMin, inMax, outMin, outMax); }
float saturate(float value) { return clamp(value, 0.0, 1.0); }
float lerp(float value0, float value1, float amount) { return mix(value0, value1, amount); }

float sampleCloudHeight(float height, float cloudType)
{
    vec2 heightUV = vec2(0.5, 1.0 - height);
    float stratus = texture(stratusHeightTexture, heightUV).r;
    float cumulus = texture(cumulusHeightTexture, heightUV).r;
    float cumulonimbus = texture(cumulonimbusHeightTexture, heightUV).r;
    if (cloudType < 0.5) return mix(stratus, cumulus, cloudType * 2.0);
    return mix(cumulus, cumulonimbus, (cloudType - 0.5) * 2.0);
}

float sampleBaseReduction(float height)
{
    return texture(baseReduceTexture, vec2(0.5, 1.0 - height)).r;
}

float getNoiseLod(float stepSize, float noiseScale, float textureSize)
{
    float texelFootprint = max(stepSize * noiseScale * textureSize, 1.0);
    return clamp(log2(texelFootprint), 0.0, log2(textureSize));
}

float samplePreviewField(vec3 uvw)
{
    switch (MODE)
    {
        case 0: return texture(perlinWorleyNoiseTexture, uvw).r;
        case 1: return texture(worleyNoise1, uvw).r;
        case 2: return texture(worleyNoise2, uvw).r;
        case 3: return texture(worleyNoise3, uvw).r;
        case 4: return texture(cloudMapTexture, uvw.xz).r;
        default: return 0.0;
    }
}

vec3 getNoiseWorldPosition(vec3 uvw)
{
    return (model * vec4(uvw - 0.5, 1.0)).xyz;
}

vec3 getAnimatedNoiseWorldPosition(vec3 uvw)
{
    vec3 noiseWorldPosition = getNoiseWorldPosition(uvw);
    vec3 normalizedWindDirection = length(windDirection) > EPSILON ? normalize(windDirection) : vec3(1.0, 0.0, 0.0);
    noiseWorldPosition += uvw.y * normalizedWindDirection * cloudTopOffset;
    noiseWorldPosition += (normalizedWindDirection + vec3(0.0, 0.1, 0.0)) * time * cloudSpeed;
    return noiseWorldPosition;
}

vec4 sampleLowFrequencyNoises(vec3 noiseWorldPosition, float stepSize)
{
    float shapeLod = getNoiseLod(stepSize, shapeScale, 128.0);
    vec3 noiseUVW = noiseWorldPosition * shapeScale;
    return vec4(textureLod(perlinWorleyNoiseTexture, noiseUVW, shapeLod).r, textureLod(worleyNoise1, noiseUVW, shapeLod).r, textureLod(worleyNoise2, noiseUVW, shapeLod).r, textureLod(worleyNoise3, noiseUVW, shapeLod).r);
}

float buildBaseCloud(vec3 uvw, float stepSize)
{
    vec4 cloudMap = texture(cloudMapTexture, uvw.xz);
    vec3 noiseWorldPosition = getAnimatedNoiseWorldPosition(uvw);
    vec4 lowFrequencyNoises = sampleLowFrequencyNoises(noiseWorldPosition, stepSize);
    float lowFrequencyFBM = dot(lowFrequencyNoises.gba, vec3(0.625, 0.250, 0.125));
    float baseCloud = clamp(remap(lowFrequencyNoises.r, -(1.0 - lowFrequencyFBM), 1.0, 0.0, 1.0), 0.0, 1.0);
    baseCloud *= sampleCloudHeight(uvw.y, cloudMap.b);

    float anvilAmount = clamp(remap(uvw.y, 0.7, 0.8, 0.0, 1.0), 0.0, 1.0);
    float anvilExponent = mix(1.0, mix(1.0, 0.5, anvilBias), anvilAmount);
    float cloudCoverage = pow(clamp(mix(cloudMap.r, cloudMap.g, cloudCoverageBlend), 0.0, 1.0), anvilExponent);
    float baseCloudWithCoverage = clamp(remap(baseCloud, 1.0 - cloudCoverage, 1.0, 0.0, 1.0), 0.0, 1.0);
    return baseCloudWithCoverage * cloudCoverage;
}

float buildLowResolutionCloud(vec3 uvw, float stepSize)
{
    return buildBaseCloud(uvw, stepSize) * sampleBaseReduction(uvw.y);
}

float buildErodedCloud(vec3 uvw, float stepSize)
{
    float baseCloud = buildBaseCloud(uvw, stepSize);
    vec3 detailUVW = getAnimatedNoiseWorldPosition(uvw) * detailScale;
    float detailLod = getNoiseLod(stepSize, detailScale, 32.0);
    vec3 highFrequencyNoises = textureLod(highFrequencyNoiseTexture, detailUVW, detailLod).rgb;
    float highFrequencyFBM = dot(highFrequencyNoises, vec3(0.625, 0.250, 0.125));
    float highFrequencyNoiseModifier = mix(highFrequencyFBM, 1.0 - highFrequencyFBM, clamp(uvw.y * 10.0, 0.0, 1.0));
    float erodedCloud = clamp(remap(baseCloud, highFrequencyNoiseModifier * erosionStrength, 1.0, 0.0, 1.0), 0.0, 1.0);
    return erodedCloud * sampleBaseReduction(uvw.y);
}

float buildLayeredCubeNoise(vec3 uvw)
{
    vec3 noiseUVW = uvw * cubeNoiseScale;
    float baseNoise = textureLod(perlinWorleyNoiseTexture, noiseUVW, 0.0).r;
    float worley1 = textureLod(worleyNoise1, noiseUVW, 0.0).r;
    float density = baseNoise - worley1 * cubeDetailStrength;
    float bottomFade = clamp(uvw.y / max(cubeBottomFade, EPSILON), 0.0, 1.0);
    float topFade = clamp((1.0 - uvw.y) / max(cubeTopFade, EPSILON), 0.0, 1.0);
    density *= bottomFade * topFade;
    return smoothstep(cubeDensityThreshold, cubeDensityThreshold + max(cubeEdgeSoftness, EPSILON), density);
}

vec3 GetViewDir() {
    vec2 screenNDC = screenUV * 2.0 - 1.0;
    vec4 viewPos = invProjection * vec4(screenNDC, 1.0, 1.0);
    vec3 viewDir = normalize(viewPos.xyz / viewPos.w);
    vec3 worldDir = normalize((invView * vec4(viewDir, 0.0)).xyz);
    return worldDir;
}

float remap(float value, float inMin, float inMax, float outMin, float outMax)
{
    return outMin + (value - inMin) * (outMax - outMin) / max(inMax - inMin, EPSILON);
}

float interleavedGradientNoise(vec2 pixelPosition)
{
    return fract(52.9829189 * fract(dot(pixelPosition, vec2(0.06711056, 0.00583715))));
}

float sampleCloudDensity(vec3 localPos, float stepSize, bool doCheaply)
{
    vec3 uvw = localPos + 0.5;
    if (any(lessThan(uvw, vec3(0.0))) || any(greaterThan(uvw, vec3(1.0))))
        return 0.0;

    float density = 0.0;
    if (MODE >= 0 && MODE <= 4) density = samplePreviewField(uvw);
    else if (MODE == 5) density = buildLowResolutionCloud(uvw, stepSize);
    else if (MODE == 6) density = doCheaply ? buildLowResolutionCloud(uvw, stepSize) : buildErodedCloud(uvw, stepSize);
    else if (MODE == 7) density = buildLayeredCubeNoise(uvw);
    return density * densityScale;
}

float cloudDensity(vec3 localPos, float stepSize)
{
    return sampleCloudDensity(localPos, stepSize, false);
}

float henyeyGreenstein(float g, float mu)
{
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * PI * pow(max(1.0 + g2 - 2.0 * g * mu, EPSILON), 1.5));
}

// Returns tNear/tFar for the cube [-0.5, 0.5].
vec2 calculateVolumeIntersection(vec3 rayOrigin, vec3 rayDirection)
{
    vec3 t0 = (-0.5 - rayOrigin) / rayDirection;
    vec3 t1 = ( 0.5 - rayOrigin) / rayDirection;
    vec3 nearValues = min(t0, t1);
    vec3 farValues = max(t0, t1);
    return vec2(max(max(nearValues.x, nearValues.y), nearValues.z), min(min(farValues.x, farValues.y), farValues.z));
}

// Integrate optical depth from the current sample toward the overhead light.
// localLightDirection is intentionally not normalized: its parameter t remains
// a world-space distance even when the cloud box is non-uniformly scaled.
float traceLightDensity(vec3 position, out float lightStepSize)
{
    vec3 worldLightDirection = normalize(lightDirection);
    vec3 localLightDirection = (inverseModel * vec4(worldLightDirection, 0.0)).xyz;

    vec2 hit = calculateVolumeIntersection(position, localLightDirection);
    float t0 = max(hit.x, 0.0);
    float t1 = hit.y;
    if (t1 <= t0) {
        lightStepSize = 1.0;
        return 0.0;
    }

    int stepCount = clamp(lightSteps, 1, 64);
    lightStepSize = (t1 - t0) / float(stepCount);
    vec3 p = position + localLightDirection * (t0 + lightStepSize * 0.5);
    float densityAlongLightRay = 0.0;

    for (int i = 0; i < 64; ++i)
    {
        if (i >= stepCount)
            break;

        densityAlongLightRay += sampleCloudDensity(p, lightStepSize, true) * (6.0 / float(stepCount));
        p += localLightDirection * lightStepSize;
    }

    return densityAlongLightRay;
}

float getScatteringMu(vec3 position)
{
    vec3 sampleWorldPosition = (model * vec4(position, 1.0)).xyz;
    vec3 directionToCamera = normalize(cameraPos - sampleWorldPosition);
    vec3 incomingLightDirection = -normalize(lightDirection);
    return clamp(dot(incomingLightDirection, directionToCamera), -1.0, 1.0);
}

float GetLightEnergy(vec3 p, float height_fraction, float dl, float ds_loded, float phase_probability, float cos_angle, float step_size, float brightness)
{
    float primary_attenuation = exp(-dl);
    float secondary_attenuation = exp(-dl * 0.25) * 0.7;
    float attenuation_probability = saturate(max(Remap(cos_angle, 0.7, 1.0, secondary_attenuation, secondary_attenuation * 0.25), primary_attenuation));
    float height_exponent = lerp(0.5, 2.0, saturate(Remap(height_fraction, 0.3, 0.85, 0.0, 1.0)));
    float depth_probability = saturate(lerp(0.05 + pow(saturate(ds_loded), height_exponent), 1.0, saturate(dl / step_size)));
    float vertical_probability = pow(saturate(Remap(height_fraction, 0.17, 0.27, 0.1, 1.0)), 0.8);
    float in_scatter_probability = depth_probability * vertical_probability;
    float light_energy = attenuation_probability * in_scatter_probability * phase_probability * brightness;
    return light_energy;
}

vec3 evaluateNubisLighting(vec3 position, float stepSize)
{
    float lightStepSize = 1.0;
    float dl = traceLightDensity(position, lightStepSize);
    float dsLoded = sampleCloudDensity(position, stepSize * 4.0, true);
    float heightFraction = position.y + 0.5;
    float cosAngle = getScatteringMu(position);
    float phaseProbability = henyeyGreenstein(phaseG, cosAngle);
    float lightEnergy = GetLightEnergy(position, heightFraction, dl, dsLoded, phaseProbability, cosAngle, lightStepSize, lightIntensity);
    return ambientLight + lightColor * lightEnergy;
}

// Raw 3D-noise preview used by MODE 0-3. Keep the original grayscale
// self-emission behavior so these textures can be inspected without lighting.
void AccumulateEmission(
    vec3 position,
    float stepSize,
    inout float transmittance,
    inout vec3 radiance)
{
    float density = cloudDensity(position, stepSize);
    float sampleT = exp(-density * stepSize * extinction);

    radiance += transmittance * vec3(density) * stepSize;
    transmittance *= sampleT;
}

void AccumulateNubisLighting(
    vec3 position,
    float stepSize,
    inout float transmittance,
    inout vec3 radiance)
{
    float density = cloudDensity(position, stepSize);
    if (density <= 0.0)
        return;

    float sampleSigmaE = max(extinction * density, EPSILON);
    float sampleSigmaS = sampleSigmaE; // Unit scattering albedo.
    float sampleT = exp(-sampleSigmaE * stepSize);
    vec3 luminance = evaluateNubisLighting(position, stepSize) * sampleSigmaS;

    radiance += transmittance * (luminance - luminance * sampleT) / sampleSigmaE;
    transmittance *= sampleT;
}

void main()
{
    vec3 rayOrigin = (inverseModel * vec4(cameraPos, 1.0)).xyz;
    vec3 worldRayDirection = GetViewDir();
    vec3 rayDirection = (inverseModel * vec4(worldRayDirection, 0.0)).xyz;

    vec2 hit = calculateVolumeIntersection(rayOrigin, rayDirection);
    float t0 = max(hit.x, 0.0);
    float t1 = hit.y;
    if (t1 <= t0) discard;

    float stepSize = (t1 - t0) / float(max(maxSteps, 1));
    float rayOffset = mix(0.5, interleavedGradientNoise(gl_FragCoord.xy), rayJitterStrength);
    vec3 position = rayOrigin + rayDirection * (t0 + stepSize * rayOffset);
    float transmittance = 1.0;
    vec3 radiance = vec3(0.0);

    for (int i = 0; i < 96; ++i)
    {
        if (i >= maxSteps || transmittance < transmittanceCutoff) break;

        if (MODE >= 0 && MODE <= 3)
            AccumulateEmission(position, stepSize, transmittance, radiance);
        else
            AccumulateNubisLighting(position, stepSize, transmittance, radiance);

        position += rayDirection * stepSize;
    }

    float alpha = clamp(1.0 - transmittance, 0.0, 1.0);
    FragColor = vec4(radiance, alpha);
}
