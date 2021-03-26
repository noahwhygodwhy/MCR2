


#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
using namespace glm;
using namespace std;


string fileToString(string filename)
{
    //printf("opening file %s\n", filename.c_str());
    string content;
    int length;
    ifstream stream(filename);
    stream.seekg(0, ios::end);
    length = stream.tellg();
    stream.seekg(0, ios::beg);
    content.resize(length);
    stream.read(content.data(), length);
    //printf("returning\n");
    //printf("%s\n", content.c_str());
    return content;
    //stream.close();
    //return content;
}

//constructs a vertex shader from the file at the path of <filename>
unsigned int makeVertexShader(string filename)
{
    string vertexShaderSource = fileToString(filename);
    const GLchar* const numChar = vertexShaderSource.data();
    //printf("%s\n", vertexShaderSource.c_str());
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &numChar, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        exit(-1);
    }
    else
    {
        cout << "SHADER::VERTEX:COMPILATION_SUCCEEDED\n" << infoLog << endl;
    }
    return vertexShader;
}

//constructs a vertex shader from the file at the path of <filename>
unsigned int makeFragmentShader(string filename)
{
    string fragmentShaderSource = fileToString(filename);
    const GLchar* const numChar = fragmentShaderSource.data();
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &numChar, NULL);
    glCompileShader(fragmentShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
        exit(-1);
    }
    else
    {
        cout << "SHADER::FRAGMENT:COMPILATION_SUCCEEDED\n" << infoLog << endl;
    }
    return fragmentShader;
}


//an object that has a vertex shader and frag shader for easier handling
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    vertShader = makeVertexShader(vertexPath);
    fragShader = makeFragmentShader(fragmentPath);
    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}
Shader::Shader()
{
    //    //vertShader = makeVertexShader("vertShader.glsl");
    //    //fragShader = makeFragmentShader("fragShader.glsl");
    //    //program = glCreateProgram();
    //    //glAttachShader(program, vertShader);
    //    //glAttachShader(program, fragShader);
    //    //glLinkProgram(program);
    //    //glDeleteShader(vertShader);
    //    //glDeleteShader(fragShader);
}

void Shader::use()
{
    glUseProgram(program);
}


//ways of setting a value in the shader without direct access to the shader. Kind of broad
//access but ya know, it seems to work so i'm not going to touch it.
void Shader::setBool(const string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}
void Shader::setInt(const string& name, int value) const
{
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::setFloat(const string& name, float value) const
{
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::setMatFour(const string& name, mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, false, value_ptr(value));
}
void Shader::setVecThree(const string& name, vec3 value) const
{
    glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, value_ptr(value));
    //glUniform3fv()
}
