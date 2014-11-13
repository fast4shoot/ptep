#version 330

uniform mat4 mvp;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec4 varPosition;
out vec3 varNormal;
out vec2 varTexCoord;

void main()
{
	gl_Position = mvp * inPosition;
	varPosition = inPosition;
	varNormal = inNormal;
	varTexCoord = inTexCoord;
}
