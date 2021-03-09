/*! \file Shader.hpp
*   \brief Contains the Shader class.
*/
#pragma once
#ifndef SHADER_H
#define SHADER_H
#include "glad.h"

#include "Asset.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
using namespace glm;

/*! \class Shader
* A class to handle the reading, compilation, and use of glsl shaders.
*/
class Shader
{
    unsigned int vertShader; ///<The pointer to the vertex shader.
    unsigned int fragShader; ///<The pointer to the fragment shader.
    unsigned int geoShader; ///< Not used yet(?).
public:
    unsigned int program; ///<The pointer to the gl shader program.
    /*! \fn Shader
    * \param vertexPath The path to the file containing the vertex shader source.
    * \param fragmentPath The path to the file containing the fragment shader source.
    */
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader();

    /*! \fn use
    * A convenience function to make OpenGL "use" this shader program.
    */
    void use();
    
    /*! \fn setBool
    * Sets a boolean uniform with the name to the value.
    * \param name The name of the uniform.
    * \param value The value to set it to. 
    */
    void setBool(const string& name, bool value) const;

    /*! \fn setInt
    * Sets an integer uniform with the name to the value.
    * \param name The name of the uniform.
    * \param value The value to set it to.
    */
    void setInt(const string& name, int value) const;

    /*! \fn setFloat
    * Sets an float uniform with the name to the value.
    * \param name The name of the uniform.
    * \param value The value to set it to.
    */
    void setFloat(const string& name, float value) const;

    /*! \fn setMatFour
    * Sets an 4x4 float matrix uniform with the name to the value.
    * \param name The name of the uniform.
    * \param value The value to set it to.
    */
    void setMatFour(const string& name, mat4 value) const;

    /*! \fn setVecThree
    * Sets an 3 element float vector uniform with the name to the value.
    * \param name The name of the uniform.
    * \param value The value to set it to.
    */
    void setVecThree(const string& name, vec3 value) const;

private:
};

#endif