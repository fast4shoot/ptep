vec3 Mraky(vec2 uv){
	vec3 barva1 = vec3(0.8, 0.8, 0.8);
	vec3 barva2 = vec3(0.2, 0.2, 0.2);
	vec3 barva3 = vec3(0.12, 0.56, 1.0);
	
	float k = 2;
	float p = snoise(k * uv) + snoise(2 * k * uv) / 2 + 
	           snoise(4 * k * uv) / 4 + snoise(8 * k * uv) / 8 + 
	           snoise(16 * k * uv) / 16 + snoise(32 * k * uv) / 32;
	vec3 barva11 = mix(barva1, barva2, p);
	
	vec3 barva21;
	barva21 = mix(
		barva3 + barva11 * 0.15,
		barva11 + ((1 - barva11) / 2),// + 0.1,
		smoothstep(2.5, 2.7, barva11[0] + barva11[1] + barva11[2])
	);
	
	return barva21;
}

Material getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	float krok = 0.0002;
	float rozsah = 5;
	vec3 obloha	= Mraky(uv);

	Material ret;
	ret.color = vec4(obloha, 1.0);
	ret.normal = vec3(0.0, 0.0, 1.0);
	ret.shininess = 3.0;
	ret.diffuseAmt = 1.0;
	ret.specularAmt = 0.2;
	
	return ret;
}
