Material getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	Material ret;
	ret.color = vec4(0.4, 0.5, 1.0, 1.0);
	ret.normal = vec3(0.0, 0.0, 1.0);
	ret.shininess = 30.0;
	ret.diffuseAmt = 1.0;
	ret.specularAmt = 1.3;
	
	return ret;
}
