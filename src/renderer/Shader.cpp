#include "renderer/Shader.h"

std::string ReadFromShader(const char* shaderpath) {
    std::string shaderCode;
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        shaderFile.open(shaderpath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
        LOG_INFO(Shader, "Suceess import shader: {}.", shaderpath);
    }
    catch (std::ifstream::failure& e)
    {
        LOG_ERROR(Shader, "Failed to read shader file: {}! {}", shaderpath, e.what());
    }
    return shaderCode;
}

struct shaderId {
    unsigned int vShaderId;
    unsigned int fShaderId;
};

unsigned int CompileShader(const char* shaderCode, const char* shaderpath, GLenum shaderType) {
    char infoLog[512];
    int success;
    unsigned int shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId);
    // compile error
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        LOG_ERROR(Shader, "Failed to compile shader: {}!", shaderpath);
    }
    else {
        LOG_INFO(Shader, "Successfully compiled shader: {}.", shaderpath);
    }
    return shaderId;
}

unsigned int LinkShader(
    unsigned int vShaderId, unsigned int fShaderId,
    const char* vShaderPath, const char* fShaderPath
) {
    char infoLog[512];
    int success;
    unsigned int programId = glCreateProgram();
    glAttachShader(programId, vShaderId);
    glAttachShader(programId, fShaderId);
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        LOG_ERROR(Shader, "Failed to link shaders: {}, {}.", vShaderPath, fShaderPath);
    }
    else {
        LOG_INFO(Shader, "Successfully linked shaders: {} and {}.", vShaderPath, fShaderPath);
    }
    glDeleteShader(vShaderId);
    glDeleteShader(fShaderId);
    return programId;
}

Shader::Shader(const char* vShaderPath, const char* fShaderPath)
{
    // get vertex
    std::string vShaderCode_str = ReadFromShader(vShaderPath);
    std::string fShaderCode_str = ReadFromShader(fShaderPath);

    const char* vShaderCode = vShaderCode_str.c_str();
    const char* fShaderCode = fShaderCode_str.c_str();

    // vertex shader compiling
    unsigned int vShaderId = CompileShader(vShaderCode, vShaderPath, GL_VERTEX_SHADER);
    unsigned int fShaderId = CompileShader(fShaderCode, fShaderPath, GL_FRAGMENT_SHADER);

    ID = LinkShader(vShaderId, fShaderId, vShaderPath, fShaderPath);
};

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec3(const std::string& name, glm::vec3 value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}
void Shader::setMat3(const std::string& name, glm::mat3 value) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setMat4(const std::string& name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}