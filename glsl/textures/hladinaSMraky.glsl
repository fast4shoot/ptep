uniform float time;

Material getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	vec3 barva1 = vec3(0.8, 0.8, 0.8);
	vec3 barva2 = vec3(0.4, 0.4, 0.4);
	vec3 barva3 = vec3(0.12, 0.56, 1.0);
	
	float modTime = time + sin(snoise(uv * 0.2));
	
	uv += modTime * 0.05;
	
	float k = 2 + time * 0.0001;
	float p = snoise(k * uv) + snoise(2 * k * uv + uv * modTime * 0.1) / 2 + 
	           snoise(4 * k * uv) / 4 + snoise(8 * k * uv) / 8 + 
	           snoise(16 * k * uv) / 16 + snoise(32 * k * uv) / 32;
	vec3 barva11 = mix(barva2, barva1, p);
	
	vec3 barva21;
	barva21 = mix(
		barva11 + ((1 - barva11) / 1.7) + 0.25,
		barva3 - barva11 * 0.04,
		smoothstep(1.0, 1.4, barva11[0] + barva11[1] + barva11[2])
	);
	
	Material ret;
	ret.color = vec4(barva21, 1.0);
	ret.normal = vec3(0.0, 0.0, 1.0);
	ret.shininess = 3.0;
	ret.diffuseAmt = 1.0;
	ret.specularAmt = 0.1;
	ret.ambientAmt = 0.3;
	
	return ret;
}
