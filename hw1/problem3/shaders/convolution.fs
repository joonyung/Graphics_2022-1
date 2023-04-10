#version 330 core
// declare input and output
out vec4 FragColor;

in vec2 textureCoords;

uniform sampler2D screenTexture;

void main()
{
    const float offset = 1.0f / 320.f;
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), 
        vec2( 0.0f,    offset), 
        vec2( offset,  offset), 
        vec2(-offset,  0.0f), 
        vec2( 0.0f,    0.0f), 
        vec2( offset,  0.0f), 
        vec2(-offset, -offset),
        vec2( 0.0f,   -offset),
        vec2( offset, -offset)
    );

    float convKernel[9] = float[](
        1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f,
        2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f,
        1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f 
    );

    vec3 sampleTexel[9];
    for(int i = 0; i < 9; i++){
        sampleTexel[i] = vec3(texture(screenTexture, textureCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0f);
    for(int i = 0; i < 9; i++){
        col += sampleTexel[i] * convKernel[i];
    }

    FragColor = vec4(col, 1.0f);   
}