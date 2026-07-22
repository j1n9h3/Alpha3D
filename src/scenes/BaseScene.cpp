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

void BaseScene::RenderFullscreenTriangle()
{
    // Vertex positions and UVs are generated from gl_VertexID in the shader.
    // OpenGL core profile still requires an object bound to GL_VERTEX_ARRAY.
    static unsigned int fullscreenVAO = 0;
    if (fullscreenVAO == 0)
        glGenVertexArrays(1, &fullscreenVAO);

    glBindVertexArray(fullscreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
