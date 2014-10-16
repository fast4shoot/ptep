#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 colour;

out vec3 vertColour;
out vec3 vertPos;

void main()
{
	gl_Position = position;	
	vertPos = position.xyz;
	vertColour = colour;
}
