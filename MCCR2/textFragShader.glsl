#version 330 core

out vec4 FragColor;

in vec2 frag_uv;
uniform sampler2D fontTexture;


void main()
{
    vec4 color = texture(fontTexture, frag_uv);

    //color = vec4(frag_uv, vec2(0.0, 1.0));

    if(color.a <= 0.0010f)//hopefully this works?
    {
       discard;
    }
    FragColor = color;
}

