#version 330 core

in vec3 worldNormal;
in vec3 localPosition;
in vec3 worldPosition;
noperspective in vec2 screenUV;

out vec4 FragColor;

// transform and camera options
uniform mat4 model;
uniform mat4 inverseModel;
uniform mat4 invProjection;
uniform mat4 invView;
uniform vec3 cameraPos;

// light options
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 ambientLight;
uniform float lightIntensity;

// density options
uniform float densityScale;
uniform float extinction;
uniform float cloudTopOffset;
uniform float anvilBias;
uniform float cloudCoverageBlend;

// noise options
uniform float shapeScale;
uniform float detailScale;
uniform float erosionStrength;

// animation options
uniform vec3 windDirection;
uniform float cloudSpeed;
uniform float time;

// cube noise options
uniform float cubeNoiseScale;
uniform float cubeDetailStrength;
uniform float cubeDensityThreshold;
uniform float cubeEdgeSoftness;
uniform float cubeBottomFade;
uniform float cubeTopFade;

// ray marching options
uniform int maxSteps;
uniform int lightSteps;
uniform float rayJitterStrength;
uniform float transmittanceCutoff;
uniform float phaseG;

// volume textures
// R: perlin-worley noise
// G: worley noise fmb 1
// B: worley noise fmb 2
// A: worley noise fmb 3
uniform sampler3D lowFreqNoiseTex;
uniform sampler3D highFreqNoiseTex;

// cloud map
// R/G: cloud coverage probabilities
// B: cloud type probability
uniform sampler2D cloudMapTex;

// cloud height map
uniform sampler2D heightTex;


const float PI = 3.14159265359;
const float EPSILON = 0.0001;

float lerp(float value0, float value1, float amount) { return mix(value0, value1, amount); }
float remap(float value, float inMin, float inMax, float outMin, float outMax) { return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin); }
float saturate(float value) { return clamp(value, 0.0, 1.0); }

float sampleCloudHeight(float height, float cloudType)
{
    vec2 heightUV = vec2(0.5, 1.0 - height);
    float stratus = texture(heightTex, heightUV).r;
    float cumulus = texture(heightTex, heightUV).g;
    float cumulonimbus = texture(heightTex, heightUV).b;
    if (cloudType < 0.5) return mix(stratus, cumulus, cloudType * 2.0);
    return mix(cumulus, cumulonimbus, (cloudType - 0.5) * 2.0);
}

float sampleBaseReduction(float height)
{
    return texture(heightTex, vec2(0.5, 1.0 - height)).a;
}

float getNoiseLod(float stepSize, float noiseScale, float textureSize)
{
    float texelFootprint = max(stepSize * noiseScale * textureSize, 1.0);
    return clamp(log2(texelFootprint), 0.0, log2(textureSize));
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
    return textureLod(lowFreqNoiseTex, noiseUVW, shapeLod).rgba;
}

float buildBaseCloud(vec3 uvw, float stepSize)
{
    vec4 cloudMap = texture(cloudMapTex, uvw.xz);
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
    vec3 highFrequencyNoises = textureLod(highFreqNoiseTex, detailUVW, detailLod).rgb;
    float highFrequencyFBM = dot(highFrequencyNoises, vec3(0.625, 0.250, 0.125));
    float highFrequencyNoiseModifier = mix(highFrequencyFBM, 1.0 - highFrequencyFBM, clamp(uvw.y * 10.0, 0.0, 1.0));
    float erodedCloud = clamp(remap(baseCloud, highFrequencyNoiseModifier * erosionStrength, 1.0, 0.0, 1.0), 0.0, 1.0);
    return erodedCloud * sampleBaseReduction(uvw.y);
}


float interleavedGradientNoise(vec2 pixelPosition)
{
    return fract(52.9829189 * fract(dot(pixelPosition, vec2(0.06711056, 0.00583715))));
}

float sampleCloudDensity(vec3 localPos, float stepSize)
{
    vec3 uvw = localPos + 0.5;
    if (any(lessThan(uvw, vec3(0.0))) || any(greaterThan(uvw, vec3(1.0))))
        return 0.0;

    return buildErodedCloud(uvw, stepSize) * densityScale;
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

        densityAlongLightRay += sampleCloudDensity(p, lightStepSize) * (6.0 / float(stepCount));
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
    float attenuation_probability = saturate(max(remap(cos_angle, 0.7, 1.0, secondary_attenuation, secondary_attenuation * 0.25), primary_attenuation));
    float height_exponent = lerp(0.5, 2.0, saturate(remap(height_fraction, 0.3, 0.85, 0.0, 1.0)));
    float depth_probability = saturate(lerp(0.05 + pow(saturate(ds_loded), height_exponent), 1.0, saturate(dl / step_size)));
    float vertical_probability = pow(saturate(remap(height_fraction, 0.17, 0.27, 0.1, 1.0)), 0.8);
    float in_scatter_probability = depth_probability * vertical_probability;
    float light_energy = attenuation_probability * in_scatter_probability * phase_probability * brightness;
    return light_energy;
}

vec3 evaluateNubisLighting(vec3 position, float stepSize)
{
    float lightStepSize = 1.0;
    float dl = traceLightDensity(position, lightStepSize);
    float dsLoded = sampleCloudDensity(position, stepSize * 4.0);
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
    float density = sampleCloudDensity(position, stepSize);
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
    float density = sampleCloudDensity(position, stepSize);
    if (density <= 0.0)
        return;

    float sampleSigmaE = max(extinction * density, EPSILON);
    float sampleSigmaS = sampleSigmaE; // Unit scattering albedo.
    float sampleT = exp(-sampleSigmaE * stepSize);
    vec3 luminance = evaluateNubisLighting(position, stepSize) * sampleSigmaS;

    radiance += transmittance * (luminance - luminance * sampleT) / sampleSigmaE;
    transmittance *= sampleT;
}


vec3 GetViewDir() {
    vec2 screenNDC = screenUV * 2.0 - 1.0;
    vec4 viewPos = invProjection * vec4(screenNDC, 1.0, 1.0);
    vec3 viewDir = normalize(viewPos.xyz / viewPos.w);
    vec3 worldDir = normalize((invView * vec4(viewDir, 0.0)).xyz);
    return worldDir;
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
        AccumulateNubisLighting(position, stepSize, transmittance, radiance);
        position += rayDirection * stepSize;
    }

    float alpha = clamp(1.0 - transmittance, 0.0, 1.0);
    FragColor = vec4(radiance, alpha);
}
