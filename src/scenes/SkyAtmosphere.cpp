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
    cubeMesh.Draw();
}

void SkyAtmosphere::Unload()
{
    scene.Clear();
}


void SkyAtmosphere::RenderEditor(Editor& editor)
{

    BaseScene::RenderEditor(editor);
}