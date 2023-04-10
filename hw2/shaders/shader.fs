#version 330 core
// TODO: define in/out and uniform variables.
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;


void main()
{
    // fill in
    // Hint) you can ignore transparent texture pixel by 
    // if(color.a < 0.5){discard;}
    vec4 color = texture(texture1, TexCoord);
    if (color.a < 0.5) {
        discard;
    }
    else FragColor = color;

}