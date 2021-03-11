#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec2 tintUV;
layout (location = 3) in float texIndex;
//layout (location = 3) in vec3 tintColor;

uniform mat4 transform;
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
    gl_Position = projection*view*transform*vec4(aPos, 1.0);
}

