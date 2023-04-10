#version 330 core
// TODO: define in/out and uniform variables.

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	// fill in
	TexCoord = aTexCoord;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	
}
