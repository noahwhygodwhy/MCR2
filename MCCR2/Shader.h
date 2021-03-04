#pragma once

#ifndef SHADER_H
#define SHADER_H
#include "glad.h"

#include "GLSLReader.h"
#include "Asset.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
using namespace glm;

class Shader
{
    unsigned int program;
    unsigned int vertShader;
    unsigned int fragShader;
    unsigned int geoShader;
public: 
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader();

    void use();

    void setBool(const string& name, bool value) const;
    void setInt(const string& name, int value) const;
    void setFloat(const string& name, float value) const;
    void setMatFour(const string& name, mat4 value) const;
    void setVecThree(const string& name, vec3 value) const;
private:
};

#endif