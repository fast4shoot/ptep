#version 330

in vec4 varPosition;
in vec3 varNormal;
in vec2 varTexCoord;

out vec4 outColor;

vec4 getColorForPoint(vec4 position, vec3 normal, vec2 uv);

void main()
{
	outColor = getColorForPoint(varPosition, varNormal, varTexCoord);
}
