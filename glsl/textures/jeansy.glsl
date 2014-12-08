Material getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	// "jeansy"
	vec3 barva1 = vec3(0.6, 0.6, 0.65);
	vec3 barva2 = vec3(0.1, 0.1, 0.25);
	vec3 barva3 = vec3(0.2, 0.2, 0.5);
	
	vec3 barva11 = mix(barva2, barva1, snoise(vec2(uv.x * 0.5, uv.y * 200)));
	vec3 barva12 = mix(barva2, barva3, snoise(vec2(uv.y, uv.x * 900)));
	vec3 barva13 = mix(barva2, barva1, 0.5);
	vec3 barva21 = mix(barva11, barva12, 0.5);
	vec3 barva31 = mix(barva21, barva13, 0.5);
	
	Material ret;
	ret.color = vec4(barva31, 1.0);
	ret.normal = vec3(0.0, 0.0, 1.0);
	ret.shininess = 3.0;
	ret.diffuseAmt = 1.0;
	ret.specularAmt = 0.2;
	
	return ret;
}
