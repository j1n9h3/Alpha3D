#pragma once
#include <glad/glad.h>

// Viewport.h
class Viewport {
public:
    void Init(int width, int height);
    void Resize(int width, int height);
    void BeginRender();
    void EndRender();
    GLuint GetFBOId() const { return this->fboID; }
    GLuint GetColorTexture() const { return colorTexture; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }

private:
    GLuint fboID = 0;
    GLuint colorTexture = 0;
    GLuint depthTexture = 0;
    float width = 0, height = 0;
};