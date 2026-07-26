// scenes/BaseScene.cpp
#include <glad/glad.h>
#include "stb_image_write.h"

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


void BaseScene::RenderLUT(GLuint lutTexture, int width, int height, GLuint framebuffer, Shader& shader, std::function<void(Shader&)> setUniforms)
{
    GLint previousFramebuffer = 0;
    GLint previousViewport[4] = {};
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glGetIntegerv(GL_VIEWPORT, previousViewport);

    glBindTexture(GL_TEXTURE_2D, lutTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lutTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
        glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
        return;
    }

    glViewport(0, 0, width, height);
    glDisable(GL_DEPTH_TEST);

    shader.use();
    setUniforms(shader);

    glClear(GL_COLOR_BUFFER_BIT);
    RenderFullscreenTriangle();

    if (depthTestEnabled) glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
    glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
}

bool BaseScene::SaveTextureLUT(GLuint texture, int width, int height, const std::string& fileName) const
{
    if (texture == 0) return false;

    GLint previousTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

    std::vector<float> pixels(width * height * 3);
    glBindTexture(GL_TEXTURE_2D, texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels.data());
    glBindTexture(GL_TEXTURE_2D, previousTexture);

    std::vector<unsigned char> preview(pixels.size());
    for (int y = 0; y < height; ++y) {
        const int flippedY = height - 1 - y;
        for (int x = 0; x < width * 3; ++x) {
            const float value = pixels[y * width * 3 + x];
            preview[flippedY * width * 3 + x] = static_cast<unsigned char>(std::clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
        }
    }

    const std::filesystem::path path = std::filesystem::path(lut_path) / GetName() / fileName;
    if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path());

    return stbi_write_png(path.string().c_str(), width, height, 3, preview.data(), width * 3) != 0;
}
