#version 330 core


//layout (location = 2) in float texIndex;

out vec4 FragColor;

//uniform int textureIndex;
uniform int layerCount;
//uniform vec3 colorChoice;

in vec3 fragPos;
in vec2 texCoord;
in vec2 tintCoord;
in float texID;
//in vec3 passedTint;
//uniform sampler2D ourTexture
uniform sampler2D foliageColorMap;


uniform sampler2DArray largeTextureStack;

void main()
{
//    float actualLayer = max(0, min(layerCount-1, floor(texID + 0.5f)));
//    FragColor = vec4(colorChoice, 1.0f);
    vec4 color = texture(largeTextureStack, vec3(texCoord, texID));// * vec4(passedTint, 1.0f
    vec4 fc = texture(foliageColorMap, tintCoord);

    if(tintCoord.x >= 0.0)
    {
        color *= fc;
        //color = vec4(1.0, 1.0, 0.0, 1.0);
    }


    /*vec4 fc = texture(foliageColorMap, tintCoord);


    if(fragPos.x < 0)
    {
        FragColor = color;
    }
    else
    {
        FragColor = fc;
    }*/



    //fc = vec4(0.0, 1.0, 0.0, 1.0);
    //FragColor = color*fc;
    //fc.a = 1.0;
    
    //color = vec4(1.0, 0.0, 0.0, 1.0);
    //if(fc.a == 1.0)
    //{
        //color = vec4(1.0, 0.0, 0.0, 1.0);
    //}

    //color.a = 1.0;
    //vec4 fc = texture(foliageColorMap, tintCoord);// * vec4(passedTint, 1.0f);
    //fc.a = 1.0;

    //vec4 abFoCo = vec4(fc.r, fc.g, fc.b, 1.0) ;
    //color = fc*color;
    //color += abFoCo;
    //if(tintCoord.x != -1)
    //{
    //    color = fc;
    //}
        /*vec4 foliageColor = vec4(texture(foliageColorMap, vec2(0.1, 0.1)).rgb, 1.0);

        color *= foliageColor;
        
        if(foliageColor.r == 0.0 && foliageColor.g == 0.0 && foliageColor.b == 0.0)
        {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
        color = vec4(1.0, 0.0, 0.0, 1.0);
        */
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
    if(color.a <= 0.0010f)//hopefully this works?
    {
        discard;
    }
    FragColor = color;
    //FragColor = color;
}

