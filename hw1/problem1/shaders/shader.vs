#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec4 moveCoord;
uniform vec2 CosSin;

void main()
{   
    float xCoord = CosSin[0] * aPos[0] + CosSin[1] * aPos[1];
    float yCoord = -CosSin[1] * aPos[0] + CosSin[0] * aPos[1];
    gl_Position = vec4(xCoord, yCoord, aPos[2], 1.0) + moveCoord;
}