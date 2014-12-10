float linearsmoothstep(float edge0, float edge1, float x)
{
	return clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
}

vec2 linearsmoothstep(float edge0, float edge1, vec2 x)
{
	return clamp((x - vec2(edge0)) / (edge1 - edge0), vec2(0.0), vec2(1.0));
}

vec3 linearsmoothstep(float edge0, float edge1, vec3 x)
{
	return clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
}

vec4 linearsmoothstep(float edge0, float edge1, vec4 x)
{
	return clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
}


vec2 linearsmoothstep(vec2 edge0, vec2 edge1, vec2 x)
{
	return clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
}

vec3 linearsmoothstep(vec3 edge0, vec3 edge1, vec3 x)
{
	return clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
}

vec4 linearsmoothstep(vec4 edge0, vec4 edge1, vec4 x)
{
	return clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
}

