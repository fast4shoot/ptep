#version 330

uniform mat4 pMat;

void applyPosition(vec4 position)
{
	gl_Position = pMat * position;
}
