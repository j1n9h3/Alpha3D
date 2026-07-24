#include "scenes/SkyAtmosphere.h"
#include "core/Log.h"
#include "stb_image_write.h"
#include "renderer/RenderProfiler.h"
#include "renderer/RenderContext.h"

#include <algorithm>
#include <filesystem>
#include <vector>

namespace {
void SetAtmosphereUniforms(const Shader& shader, const SkyAtmosphereParameters& p)
{
    shader.setBool("useRayleigh", p.useRayleigh);
    shader.setBool("useMie", p.useMie);
    shader.setBool("useAbsorption", p.useAbsorption);
    shader.setFloat("planetRadius", p.planetRadius);
    shader.setFloat("atmosphereRadius", p.atmosphereRadius);
    shader.setVec3("rayleighBeta", p.rayleighBeta);
    shader.setVec3("mieBeta", p.mieBeta);
    shader.setVec3("absorptionBeta", p.absorptionBeta);
    shader.setFloat("rayleighHeight", p.rayleighHeight);
    shader.setFloat("mieHeight", p.mieHeight);
    shader.setFloat("absorptionHeight", p.absorptionHeight);
    shader.setFloat("absorptionFalloff", p.absorptionFalloff);
}
}

void SkyAtmosphere::Load(Window& window)
{
    BaseScene::Load(window); // glEnable

    glGenTextures(1, &transmittanceLUT);
    glGenFramebuffers(1, &captureFBO);

    RenderTransmittanceLUT();
    SaveTransmittanceLUT("record/transmittance_lut.png");
}

void SkyAtmosphere::Render(RenderContext& context)
{
    A3_PROFILE_PASS(context.profiler, "Atmosphere");
    Camera& camera = context.camera;
    shader_sky_atmosphere.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, transmittanceLUT);
    shader_sky_atmosphere.setInt("transmittanceLUT", 0);

    SetAtmosphereUniforms(shader_sky_atmosphere, parameters);
    shader_sky_atmosphere.setBool("useTransmittanceLUT", parameters.useTransmittanceLUT);
    shader_sky_atmosphere.setMat4("invProjection", glm::inverse(camera.GetProjection()));
    shader_sky_atmosphere.setMat4("invView", glm::inverse(camera.GetView()));
    shader_sky_atmosphere.setVec3("cameraPos", camera.GetPosition());
    shader_sky_atmosphere.setVec3("lightDirection", glm::normalize(parameters.lightDirection));
    shader_sky_atmosphere.setFloat("lightIntensity", parameters.lightIntensity);
    shader_sky_atmosphere.setFloat("cameraHeight", parameters.cameraHeight);
    shader_sky_atmosphere.setFloat("mieG", parameters.mieG);
    shader_sky_atmosphere.setFloat("exposure", parameters.exposure);
    shader_sky_atmosphere.setFloat("gamma", parameters.gamma);
    shader_sky_atmosphere.setInt("primarySteps", parameters.primarySteps);
    shader_sky_atmosphere.setInt("lightSteps", parameters.lightSteps);
    RenderFullscreenTriangle();
}

void SkyAtmosphere::RenderTransmittanceLUT() {

    GLint previousFramebuffer = 0;
    GLint previousViewport[4] = {};
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glGetIntegerv(GL_VIEWPORT, previousViewport);

    glBindTexture(GL_TEXTURE_2D, transmittanceLUT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, transmittanceLUTWidth, transmittanceLUTHeight, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, transmittanceLUT, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR(SkyAtmosphere, "Transmittance LUT framebuffer is incomplete");
        glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
        glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
        return;
    }

    glViewport(0, 0, transmittanceLUTWidth, transmittanceLUTHeight);
    glDisable(GL_DEPTH_TEST);
    shader_render_transmittance_lut.use();

    SetAtmosphereUniforms(shader_render_transmittance_lut, parameters);

    shader_render_transmittance_lut.setInt("transmittanceSteps", parameters.transmittanceSteps);
    glClear(GL_COLOR_BUFFER_BIT);
    RenderFullscreenTriangle();

    if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
    glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
}

bool SkyAtmosphere::SaveTransmittanceLUT(const std::string& outputPath) const
{
    if (transmittanceLUT == 0) return false;

    GLint previousTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
    std::vector<float> pixels(transmittanceLUTWidth * transmittanceLUTHeight * 3);
    glBindTexture(GL_TEXTURE_2D, transmittanceLUT);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels.data());
    glBindTexture(GL_TEXTURE_2D, previousTexture);

    std::vector<unsigned char> preview(pixels.size());
    float minValue = 1.0f;
    float maxValue = 0.0f;
    for (int y = 0; y < transmittanceLUTHeight; ++y) {
        const int flippedY = transmittanceLUTHeight - 1 - y;
        for (int x = 0; x < transmittanceLUTWidth * 3; ++x) {
            const float value = pixels[(y * transmittanceLUTWidth * 3) + x];
            minValue = std::min(minValue, value);
            maxValue = std::max(maxValue, value);
            preview[(flippedY * transmittanceLUTWidth * 3) + x] =
                static_cast<unsigned char>(std::clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
        }
    }

    const std::filesystem::path path(outputPath);
    if (!path.parent_path().empty()) {
        std::filesystem::create_directories(path.parent_path());
    }
    const int success = stbi_write_png(path.string().c_str(), transmittanceLUTWidth,
        transmittanceLUTHeight, 3, preview.data(), transmittanceLUTWidth * 3);
    if (!success) {
        LOG_ERROR(SkyAtmosphere, "Failed to save transmittance LUT: {}", path.string());
        return false;
    }

    LOG_INFO(SkyAtmosphere, "Transmittance LUT saved: {} (range [{}, {}])",
        path.string(), minValue, maxValue);
    return true;
}

void SkyAtmosphere::Unload()
{
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteTextures(1, &transmittanceLUT);
    captureFBO = 0;
    transmittanceLUT = 0;
    scene.Clear();
}


void SkyAtmosphere::RenderEditor(Editor& editor)
{
    editor.BeginSkyAtmosphere(*this);
    BaseScene::RenderEditor(editor);
}
