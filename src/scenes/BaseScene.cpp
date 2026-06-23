// scenes/BaseScene.cpp
#include <glad/glad.h>

#include "scenes/BaseScene.h"
#include "core/Editor.h"

void BaseScene::Load(Window& window)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void BaseScene::RenderEditor(Editor& editor)
{
    editor.BeginHierarchy(scene);
    if (scene.GetSelected()) editor.BeginDetails(*scene.GetSelected());
}