Material getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	vec2 camoUv = floor(uv * 250) * 0.004;
	
	vec3 barva0 = vec3(0.56, 0.61, 0.35);
	vec3 barva1 = vec3(0.4, 0.31, 0.22);
	vec3 barva2 = vec3(0.44, 0.44, 0.2);
	vec3 barva3 = vec3(0.85, 0.82, 0.63);
	
	float mix0 =  smoothstep(-0.01, 0.01, snoise(vec3(camoUv * 14, 465)));
	float mix1 = smoothstep(-0.01, 0.01, snoise(vec3(camoUv * 16, 12)));
	float mix0123 = smoothstep(-0.01, 0.01, snoise(vec3(camoUv * 16, 54)));
	vec3 barva01 = mix(barva0, barva1, mix0);
	vec3 barva23 = mix(barva2, barva3, mix0);
	vec3 barva0123 = mix(barva01, barva23, mix1);
	
	Material ret;
	ret.color = vec4(barva0123, 1.0) * makePositive(snoise(vec3(uv * 1050, 12)) * 0.1 + 0.9);
	ret.normal = vec3(0.0, 0.0, 1.0);
	ret.shininess = 30.0;
	ret.diffuseAmt = 1.0;
	ret.specularAmt = 0.1;
	ret.ambientAmt = 0.3;
	
	return ret;
}
