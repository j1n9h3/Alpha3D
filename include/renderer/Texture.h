// renderer/Texture.h
#pragma once
#include <string>

class Texture {
public:
    Texture(const std::string& path);
    ~Texture();
    void Bind(unsigned int slot = 0);
private:
    unsigned int m_TextureID;
    int m_Width, m_Height;
};