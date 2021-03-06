#version 330 core


//layout (location = 2) in float texIndex;

out vec4 FragColor;

//uniform int textureIndex;
uniform int layerCount;
//uniform vec3 colorChoice;

in vec2 texCoord;
in vec2 tintCoord;
in float texID;
//in vec3 passedTint;
//uniform sampler2D ourTexture
uniform sampler2D foliageColorMap;
uniform sampler2DArray ourTexture;

void main()
{
//    float actualLayer = max(0, min(layerCount-1, floor(texID + 0.5f)));
//    FragColor = vec4(colorChoice, 1.0f);
    vec4 color = texture(ourTexture, vec3(texCoord, texID));// * vec4(passedTint, 1.0f);
    if(tintCoord.x >= 0)
    {
        //color *= texture(foliageColorMap, tintCoord);
        /*color = texture(foliageColorMap, tintCoord);
        if(color == vec4(0.0, 0.0, 0.0, 0.0))
        {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
        if(tintCoord == vec2(0.2, 0.2))
        {
            color = vec4(0.0, 1.0, 0.0, 1.0);
        }*/
        //color=vec4(1.0, 0.0, 0.0, 1.0);
    }
    if(color.a <= 0.0005f)//hopefully this works?
    {
        discard;
    }
    FragColor = color;
}

