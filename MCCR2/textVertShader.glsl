#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in float id;

uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec2 tintCoord;
out float texID;
out vec3 fragPos;
//out vec3 passedTint;

void main()
{

    texID = texIndex;
    texCoord = uv;
    tintCoord = tintUV;
    fragPos = aPos;
    //passedTint = tintColor;
    gl_Position = projection*view*vec4(aPos, 1.0);
}

