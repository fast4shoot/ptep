#version 330

in GeometryToFragment
{
	vec3 color;
} input;

out vec4 outColor;

void main()
{
	outColor = vec4(input.color, 1.0);
}
