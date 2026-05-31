// renderer/Texture.cpp
#pragma once

#include "renderer/Texture.h"


#define STB_IMAGE_IMPLEMENTATION

#include "stb/stb_image.h"


unsigned int Texture::GetId() {
    return this->mId;
}

aiString Texture::GetTexName() {
    return this->mName;
}

unsigned int TextureFromFile(const char* tex_name, const std::string& directory)
{
    std::string filePath = std::string(tex_name);
    filePath = directory + '/' + filePath;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        LOG_INFO(Texture, "Texture loaded at path {}", filePath);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << tex_name << std::endl;
        LOG_ERROR(Texture, "Texture failed to load at path {}", filePath);
        stbi_image_free(data);
    }

    return textureID;
}

Texture::Texture(aiString tex_name, const std::string& directory, const std::string& typeName) {

    this->mId = TextureFromFile(tex_name.C_Str(), directory);
    this->mType = typeName;
    this->mName = tex_name.C_Str();
}

Texture::Texture(const std::string& path) {
    glGenTextures(1, &mId);
    glBindTexture(GL_TEXTURE_2D, mId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        LOG_INFO(Texture, "Texture loaded: {}", path);
    }
    else {
        LOG_ERROR(Texture, "Failed to load texture: {}", path);
    }
    stbi_image_free(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &mId);
}

void Texture::Bind(unsigned int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, mId);
}