#pragma once
#include <glad/glad.h>
#include <string>

class Shader;
class Mesh;

class IBL {
public:
    unsigned int envCubemap = 0;
    unsigned int irradianceMap = 0;
    unsigned int prefilterMap = 0;
    unsigned int brdfLUTTexture = 0;

    void Load(const std::string& hdr_path, Mesh& cubeMesh,
        Shader& to_cubemap_shader,
        Shader& irradiance_shader,
        Shader& prefilter_shader,
        Shader& brdf_integrate_shader);

    void Bind(Shader& shader) const;

    ~IBL();
private:
    unsigned int captureFBO = 0;
    unsigned int captureRBO = 0;
};