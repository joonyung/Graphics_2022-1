#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform vec3 Scale;
uniform float Rotate;
uniform vec3 Position;

void main()
{   
    vec3 initPos = Scale * aPos;
    float cosRot = cos(Rotate);
    float sinRot = sin(Rotate);
    gl_Position = vec4(cosRot * initPos[0] + sinRot * initPos[1] + Position[0], -sinRot * initPos[0] + cosRot * initPos[1] + Position[1], initPos[2] + Position[2], 1.0);
    ourColor = aColor;
    
}