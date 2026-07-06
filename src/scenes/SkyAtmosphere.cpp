#include "scenes/SkyAtmosphere.h"

void SkyAtmosphere::Load(Window& window)
{
    BaseScene::Load(window); // glEnable
}

void SkyAtmosphere::Render(Camera& camera)
{

}

void SkyAtmosphere::Unload()
{
    scene.Clear();
}


void SkyAtmosphere::RenderEditor(Editor& editor)
{

    BaseScene::RenderEditor(editor);
}