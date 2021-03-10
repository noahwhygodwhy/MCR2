#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
//layout (location = 3) in vec3 tintColor;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

out vec3 color;

void main()
{
    color = aColor;
    gl_Position = projection*transform*vec4(aPos, 1.0);
}

