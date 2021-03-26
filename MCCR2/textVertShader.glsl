#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv;

uniform mat4 projection;


out vec2 frag_uv;

void main()
{

    frag_uv = uv;
    gl_Position = projection*vec4(pos, 0.0, 1.0);
}

