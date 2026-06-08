#pragma once

#include "renderer/Model.h"

void Model::Draw(Shader& shader)
{
    for (unsigned int i = 0; i < mMeshes.size(); i++)
        mMeshes[i].Draw(shader);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString tex_name;
        mat->GetTexture(type, i, &tex_name);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].GetTexName().C_Str(), tex_name.C_Str()) == 0)
            {
                textures.push_back(std::move(textures_loaded[j]));
                skip = true;
                break;
            }
        }

        LOG_INFO(Model, "Loading Model Texture '{}'", std::string(tex_name.C_Str()));

        if (!skip)
        {
            Texture texture(tex_name, this->directory, typeName);

            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}


Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        for (int t = 0; t < AI_TEXTURE_TYPE_MAX; t++) {
            int count = material->GetTextureCount((aiTextureType)t);
            if (count > 0) {
                aiString path;
                material->GetTexture((aiTextureType)t, 0, &path);
                LOG_INFO(Model, "Consists texture type: {}: {}", t, path.C_Str());
            }
        }
    }

    // vao vertex processing
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec3 vector;

        // setting vertex position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;

        vertex.Position = vector;

        // setting vertex normal
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;

        vertex.Normal = vector;

        // setting vertex texture coords
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->mTangents) {
            vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        }

        // adding vertex
        vertices.push_back(vertex);
    }

    // ebo indices processing
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
            
    }

    // material processing
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> albedoMaps = loadMaterialTextures(material, aiTextureType_BASE_COLOR, "albedo");
        textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());

        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> roughnessMap = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "roughness");
        textures.insert(textures.end(), roughnessMap.begin(), roughnessMap.end());
    }

    //return Mesh(vertices, indices, textures);
    return Mesh(vertices, indices, std::move(textures));
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->mMeshes.push_back(std::move(processMesh(mesh, scene)));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void Model::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_ERROR(Model, import.GetErrorString());
        return;
    }
    else {
        LOG_INFO(Model, "Model loaded: {}, meshes: {}", path, scene->mNumMeshes);
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}