#pragma once

#include "Logger.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "resources/assets/Shaders.h"

char DEFAULT_VERTEX_SHADER[];
char DEFAULT_FRAGMENT_SHADER[];

char DEFAULT_SHADOW_VERTEX_SHADER[];
char DEFAULT_SHADOW_FRAGMENT_SHADER[];

char DEFAULT_VERTEX_QUAD_SHADER[];
char DEFAULT_FRAGMENT_FEATURE_SHADER[];
char DEFAULT_FRAGMENT_SCREEN_SHADER[];

char DEFAULT_FRAGMENT_BLUR_SHADER[];

class Shader
{
public:
    unsigned int ID;
    illusion::resources::assets::ShaderResource resource;

    Shader() { ID = 0; }
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const std::string& vertexCode, const std::string& fragmentCode, const std::string& geometryCode = "")
    {
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry = 0;
        if (geometryCode != "")
        {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryCode != "")
            glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryCode != "")
            glDeleteShader(geometry);

    }

    Shader(const illusion::resources::assets::ShaderResource resource) : Shader(resource.vertex, resource.fragment) {
        this->resource = resource;
    }

    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    inline void setBool(const std::string& name, const bool& value,GLsizei count=1) const
    {
        glUniform1iv(glGetUniformLocation(ID, name.c_str()),count, (int *)&value);
    }
    // ------------------------------------------------------------------------
    inline  void setInt(const std::string& name,const int& value,GLsizei count=1) const
    {
        glUniform1iv(glGetUniformLocation(ID, name.c_str()),count, &value);
    }
    // ------------------------------------------------------------------------
    inline  void setUnsignedInt(const std::string& name,const unsigned int& value,GLsizei count=1) const
    {
        glUniform1uiv(glGetUniformLocation(ID, name.c_str()),count, &value);
    }
    // ------------------------------------------------------------------------
    inline void setFloat(const std::string& name,const float& value,GLsizei count=1) const
    {
        glUniform1fv(glGetUniformLocation(ID, name.c_str()),count, &value);
    }
    // ------------------------------------------------------------------------
    inline void setVec2(const std::string& name, const glm::vec2& value,GLsizei count=1) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), count, &value[0]);
    }
    inline void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    inline void setVec3(const std::string& name, const glm::vec3& value,GLsizei count=1) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), count, &value[0]);
    }
    inline void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    inline void setVec4(const std::string& name, const glm::vec4& value,GLsizei count=1) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), count, &value[0]);
    }
    inline void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    inline void setMat2(const std::string& name, const glm::mat2& mat,GLsizei count=1) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    inline void setMat3(const std::string& name, const glm::mat3& mat,GLsizei count=1) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    inline void setMat4(const std::string& name, const glm::mat4& mat,GLsizei count=1) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, &mat[0][0]);
    }

    static void Initialize() {
        Shader::defaultShaderResource = {
            0,
            DEFAULT_VERTEX_SHADER,
            DEFAULT_FRAGMENT_SHADER,
            {
                { "diffuse", {{ "type", "Color3" }, { "default", {1,1,1} }} },
                { "specular", {{ "type", "Color3" }, { "default", {1,1,1} }} },
                { "ambient", {{ "type", "Color3" }, { "default", {1,1,1} }} },
                { "shininess", {{ "type", "f32" }, { "default", 32.0f }} },
            }
        };
        Shader::defaultShader = Shader(Shader::defaultShaderResource);
        defaultShader.use();
        defaultShader.setInt("directShadowMap", 0);

        Shader::featureShader = Shader(DEFAULT_VERTEX_QUAD_SHADER, DEFAULT_FRAGMENT_FEATURE_SHADER);
        Shader::screenShader = Shader(DEFAULT_VERTEX_QUAD_SHADER, DEFAULT_FRAGMENT_SCREEN_SHADER);

        screenShader.use();
        screenShader.setInt("scene", 0);
        screenShader.setInt("bloomBlur", 1);

        Shader::shadowShader = Shader(DEFAULT_SHADOW_VERTEX_SHADER, DEFAULT_SHADOW_FRAGMENT_SHADER);

        Shader::blurShader = Shader(DEFAULT_VERTEX_QUAD_SHADER, DEFAULT_FRAGMENT_BLUR_SHADER);
        blurShader.use();
        blurShader.setInt("image", 0);
    }

    static illusion::resources::assets::ShaderResource defaultShaderResource;
    static Shader defaultShader;
    static Shader featureShader;
    static Shader screenShader;

    static Shader shadowShader;
    static Shader blurShader;

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                ERR("ERROR::SHADER_COMPILATION_ERROR of type: ", type, "\n", infoLog, "\n -- --------------------------------------------------- -- ");
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                ERR("ERROR::PROGRAM_LINKING_ERROR of type: ", type, "\n", infoLog, "\n -- --------------------------------------------------- -- ");
            }
        }
    }
};
