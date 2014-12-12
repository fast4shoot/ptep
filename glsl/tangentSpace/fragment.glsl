#version 330

in GeometryToFragment
{
	vec3 color;
} IN;

out vec4 outColor;

void main()
{
	outColor = vec4(IN.color, 1.0);
}
