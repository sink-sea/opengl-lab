#ifndef SHADER_S_H
#define SHADER_S_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

class Shader {
public:
    GLuint programID; // Program ID

    Shader(const char* vertexPath, const char* fragmentPath); // Constructor
    void use(); // Use/activate the shader

    // Utility uniform functions
    void setBool4(const std::string&, bool = 0, bool = 0, bool = 0, bool = 1) const;
    void setInt4(const std::string&, int = 0, int = 0, int = 0, int = 1) const;
    void setInt1(const std::string&, int = 0) const;
    void setFloat1(const std::string&, float = 0.0f) const;
    void setFloat4(const std::string&, float = 0.0f, float = 0.0f, float = 0.0f, float = 1.0f) const;
    void setMat4(const std::string&, const float*) const;
    void setVec3(const std::string&, const glm::vec3&) const;
private:
    void checkCompileError(GLuint shader, std::string type);
};

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open GLSL file
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile shader
    GLuint vertex{}, fragment{};
    // --- vertex
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileError(vertex, "VERTEX");
    // --- fragment
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileError(fragment, "FRAGMENT");

    // Link shader
    programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);
    checkCompileError(programID, "PROGRAM");

    // Deallocate shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(programID);
}

void Shader::setBool4(const std::string& name, bool val1, bool val2, bool val3, bool val4) const {
    glUniform4i(glGetUniformLocation(programID, name.c_str()), (int) val1, (int) val2, (int) val3, (int) val4);
}

void Shader::setInt1(const std::string& name, int val) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), val);
}
void Shader::setInt4(const std::string& name, int val1, int val2, int val3, int val4) const {
    glUniform4i(glGetUniformLocation(programID, name.c_str()), val1, val2, val3, val4);
}

void Shader::setFloat4(const std::string& name, float val1, float val2, float val3, float val4) const {
    glUniform4f(glGetUniformLocation(programID, name.c_str()), val1, val2, val3, val4);
}

void Shader::setFloat1(const std::string& name, float val) const {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), val);
}

void Shader::setMat4(const std::string& name, const float* val) const {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, val);
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) const {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::checkCompileError(GLuint shader, std::string type) {
    GLint success{};
    char infoLog[1024]{};
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

#endif // SHADER_S_H