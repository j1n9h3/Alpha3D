#include "scenes/SkyAtmosphere.h"
#include "core/Log.h"
#include "renderer/RenderProfiler.h"
#include "renderer/RenderContext.h"

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

void SetScatteringUniforms(const Shader& shader, const SkyAtmosphereParameters& p)
{
    shader.setBool("useTransmittanceLUT", p.useTransmittanceLUT);
    shader.setBool("useSkyViewLUT", p.useSkyViewLUT);
    shader.setInt("transmittanceLUT", 0);
    shader.setInt("skyViewLUT", 1);
    shader.setFloat("lightIntensity", p.lightIntensity);
    shader.setVec3("lightDirection", glm::normalize(p.lightDirection));
    shader.setFloat("cameraHeight", p.cameraHeight);
    shader.setFloat("mieG", p.mieG);
    shader.setFloat("exposure", p.exposure);
    shader.setFloat("gamma", p.gamma);
    shader.setInt("primarySteps", p.primarySteps);
    shader.setInt("lightSteps", p.lightSteps);
}
}

void SkyAtmosphere::Load(Window& window)
{
    BaseScene::Load(window); // glEnable

    glGenTextures(1, &transmittanceLUT);
    glGenTextures(1, &skyViewLUT);
    glGenFramebuffers(1, &captureFBO);

    shader_sky_atmosphere.use();
    SetAtmosphereUniforms(shader_sky_atmosphere, parameters);
    SetScatteringUniforms(shader_sky_atmosphere, parameters);

    RenderTransmittanceLUT();
    SaveTransmittanceLUT("transmittance_lut.png");
    RenderSkyViewLUT();
    SaveSkyViewLUT("sky_view_lut.png");
    parametersDirty = false;
}

void SkyAtmosphere::Render(RenderContext& context)
{
    A3_PROFILE_PASS(context.profiler, "Atmosphere");
    Camera& camera = context.camera;
    shader_sky_atmosphere.use();
    shader_sky_atmosphere.setMat4("invProjection", glm::inverse(camera.GetProjection()));
    shader_sky_atmosphere.setMat4("invView", glm::inverse(camera.GetView()));
    shader_sky_atmosphere.setVec3("cameraPos", camera.GetPosition());
    shader_sky_atmosphere.setInt("MODE", 0);

    if (parametersDirty) {
        SetAtmosphereUniforms(shader_sky_atmosphere, parameters);
        SetScatteringUniforms(shader_sky_atmosphere, parameters);
        RenderSkyViewLUT();
        shader_sky_atmosphere.setInt("MODE", 0);
        parametersDirty = false;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, transmittanceLUT);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, skyViewLUT);
    glActiveTexture(GL_TEXTURE0);
    RenderFullscreenTriangle();
}

void SkyAtmosphere::RenderTransmittanceLUT() {

    BaseScene::RenderLUT(transmittanceLUT, transmittanceLUTWidth, transmittanceLUTHeight, captureFBO, shader_sky_atmosphere, [&](Shader& shader) {
        shader.setInt("MODE", 1);
        shader.setInt("transmittanceLUTSteps", parameters.transmittanceLUTSteps);
    });
}

void SkyAtmosphere::RenderSkyViewLUT()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, transmittanceLUT);
    glActiveTexture(GL_TEXTURE1);

    BaseScene::RenderLUT(skyViewLUT, skyViewLUTWidth, skyViewLUTHeight, captureFBO, shader_sky_atmosphere, [&](Shader& shader) {
        shader.setInt("MODE", 2);
    });

    glActiveTexture(GL_TEXTURE0);
}

bool SkyAtmosphere::SaveTransmittanceLUT(const std::string& outputPath) const
{
    return SaveTextureLUT(transmittanceLUT, transmittanceLUTWidth, transmittanceLUTHeight, outputPath);
}

bool SkyAtmosphere::SaveSkyViewLUT(const std::string& outputPath) const
{
    return SaveTextureLUT(skyViewLUT, skyViewLUTWidth, skyViewLUTHeight, outputPath);
}

void SkyAtmosphere::Unload()
{
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteTextures(1, &transmittanceLUT);
    glDeleteTextures(1, &skyViewLUT);
    captureFBO = 0;
    transmittanceLUT = 0;
    skyViewLUT = 0;
    scene.Clear();
}


void SkyAtmosphere::RenderEditor(Editor& editor)
{
    editor.BeginSkyAtmosphere(*this);
    BaseScene::RenderEditor(editor);
}
