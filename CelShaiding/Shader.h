#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID; // айди шейдера

    Shader(const char* vPath, const char* fPath) {
        // читаем файлы
        std::string vCode;
        std::string fCode;
        std::ifstream vFile;
        std::ifstream fFile;

        // чтоб ошибки ловить
        vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            vFile.open(vPath);
            fFile.open(fPath);
            std::stringstream vStream, fStream;

            vStream << vFile.rdbuf();
            fStream << fFile.rdbuf();

            vFile.close();
            fFile.close();

            vCode = vStream.str();
            fCode = fStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_READ " << e.what() << std::endl;
        }

        const char* vShaderCode = vCode.c_str();
        const char* fShaderCode = fCode.c_str();

        // компил€ци€
        unsigned int vertex, fragment;

        // вершинный
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // checkCompileErrors(vertex, "VERTEX"); 

        // фрагментный
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);

        // программа
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);

        // удал€ем ненужное
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() {
        glUseProgram(ID);
    }

    // утилиты
    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
};