#version 330 core
// TODO: define in/out and uniform variables.
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube day;
uniform samplerCube night;
uniform float dayFactor;

void main()
{   
    // mix two texture
    FragColor = mix(texture(night, TexCoords), texture(day, TexCoords), dayFactor);
}
