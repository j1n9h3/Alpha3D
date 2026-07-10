#include "scenes/SkyAtmosphere.h"

void SkyAtmosphere::Load(Window& window)
{
    BaseScene::Load(window); // glEnable
}

void SkyAtmosphere::Render(Camera& camera)
{
    shader_sky_atmosphere.use();
    shader_sky_atmosphere.setMat4("invProjection", glm::inverse(camera.GetProjection()));
    shader_sky_atmosphere.setVec3("cameraPos", camera.GetPosition());
    shader_sky_atmosphere.setVec3("lightDirection", glm::normalize(parameters.lightDirection));
    shader_sky_atmosphere.setFloat("lightIntensity", parameters.lightIntensity);
    shader_sky_atmosphere.setFloat("cameraHeight", parameters.cameraHeight);
    shader_sky_atmosphere.setFloat("planetRadius", parameters.planetRadius);
    shader_sky_atmosphere.setFloat("atmosphereRadius", parameters.atmosphereRadius);
    shader_sky_atmosphere.setVec3("rayleighBeta", parameters.rayleighBeta);
    shader_sky_atmosphere.setVec3("mieBeta", parameters.mieBeta);
    shader_sky_atmosphere.setVec3("absorptionBeta", parameters.absorptionBeta);
    shader_sky_atmosphere.setFloat("rayleighHeight", parameters.rayleighHeight);
    shader_sky_atmosphere.setFloat("mieHeight", parameters.mieHeight);
    shader_sky_atmosphere.setFloat("absorptionHeight", parameters.absorptionHeight);
    shader_sky_atmosphere.setFloat("absorptionFalloff", parameters.absorptionFalloff);
    shader_sky_atmosphere.setFloat("mieG", parameters.mieG);
    shader_sky_atmosphere.setFloat("exposure", parameters.exposure);
    shader_sky_atmosphere.setFloat("gamma", parameters.gamma);
    cubeMesh.Draw();
}

void SkyAtmosphere::Unload()
{
    scene.Clear();
}


void SkyAtmosphere::RenderEditor(Editor& editor)
{
    editor.BeginSkyAtmosphere(*this);
    BaseScene::RenderEditor(editor);
}
