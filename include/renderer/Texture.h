// renderer/Texture.h
#pragma once
#include <string>
#include <glad/glad.h>
#include <iostream>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "core/Log.h"

class Texture {
public:
    Texture(const std::string&);
    Texture(aiString, const std::string&, const std::string&);

    ~Texture();

    unsigned int GetId() { return mId; }
    unsigned int GetId() const { return mId; }
    aiString           GetTexName()const { return mName; }
    const std::string& GetType()   const { return mType; }

    void Bind(unsigned int slot = 0);
private:
    unsigned int mId;
    int width, height;
    std::string mType;
    aiString mName;
};