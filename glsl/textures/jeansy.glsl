Material getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	// "jeansy"
	vec3 barva1 = vec3(0.5, 0.5, 0.65);
	vec3 barva2 = vec3(0.14, 0.14, 0.37);
	vec3 barva3 = vec3(0.2, 0.2, 0.43);
	
	vec3 barva11 = mix(barva2, barva1, snoise(vec2(uv.x, uv.y * 300)));
	vec3 barva12 = mix(barva2, barva3, snoise(vec2(uv.y * 2, uv.x * 600)));
	vec3 barva13 = mix(barva2, barva1, snoise(vec2(uv.y * 200, uv.x * 400)));
	vec3 barva21 = mix(barva11, barva12, 0.65);
	if(snoise(vec2(uv.y * 200, uv.x * 400)) > 0.7) barva21 = mix(barva21, barva13, 0.5);
		
	Material ret;
	ret.color = vec4(barva21, 1.0);
	ret.normal = vec3(0.0, 0.0, 1.0);
	ret.shininess = 3.0;
	ret.diffuseAmt = 1.0;
	ret.specularAmt = 0.2;
	ret.ambientAmt = 0.5;
	
	return ret;
}
