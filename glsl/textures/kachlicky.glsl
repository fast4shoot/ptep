Material getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	// "kachlicky"
	float v = 10;
	vec3 barva01 = vec3(0.9, 0.9, 0.9);
	vec3 barva02 = vec3(0.06, 0.35, 0.96);
    vec3 barva03 = vec3(0.16, 0.85, 0.16);
	vec3 barva021 = barva02 * 0.4;
	//vec3 barva022 = barva02 * 0.7;
    vec3 barva022 = barva03 * 0.7;
	vec3 barva023 = barva02 * 1.4;
	vec3 barva024 = barva022 * 0.4;
	vec3 barva025 = barva022 * 1.4;
	
	// spara
	vec3 barva_a01 = mix(barva01 * 1.15, barva01 * 0.9, abs(fract(200.0 * uv.x+0.4) * 2.0 - 1.0));
	vec3 barva_a02 = mix(barva01 * 1.15, barva01 * 0.9, abs(fract(200.0 * uv.y+0.4) * 2.0 - 1.0));
	vec3 barva_a11;
	
	if(fract(v * uv.x + 0.025) < .06 && fract(v * uv.y + 0.025) < .06) 
		barva_a11 = mix(barva_a01, barva_a02, 0.5) * 1.06;
	else if(fract(v * uv.x + 0.025) < .06) barva_a11 = barva_a01;
	else if(fract(v * uv.y + 0.025) < .06) barva_a11 = barva_a02;
	else barva_a11 = mix(barva_a01, barva_a02, 0.5);
	
	vec3 barva_a12 = mix(barva01, barva01 * 0.5, 0.5 * snoise(vec2(uv.x * 3000,uv.y * 3000)));
	vec3 barva_a21 = mix(barva_a11, barva_a12, 0.2);
	// \spara
	
	// kachlicka
	barva02 = mix(barva02, barva02 * 1.15, snoise(vec2(snoise(vec2(uv.x * 200, uv.y * 10)), 2)));
	barva022 = mix(barva022, barva022 * 1.15, snoise(vec2(snoise(vec2(uv.x * 10, uv.y * 200)), 2)));
	
	barva023 = mix(barva023,barva02, fract(uv.x * 100));
	barva025 = mix(barva025,barva022, fract(uv.x * 100));
	barva021 = mix(barva02,barva021, fract(uv.x * 100));
	barva024 = mix(barva022,barva024, fract(uv.x * 100));
		
	vec3 barva_b21 = mix(barva02, barva021, step(0.45, length(fract(v * uv.x + 0.05) -0.5)));
	vec3 barva_b22 = mix(barva022, barva024, step(0.45, length(fract(v * uv.x + 0.05) -0.5)));
	vec3 barva_b23 = mix(barva_b21, barva023, step(0.45, length(fract(v * uv.x + 0.05 + 0.9) -0.5)));
	vec3 barva_b24 = mix(barva_b22, barva025, step(0.45, length(fract(v * uv.x + 0.05 + 0.9) -0.5)));

	vec3 barva_b31 = mix(barva_b23, barva_b24, mod(floor(uv.x * v)+floor(uv.y * v), 2));
	// \kachlicka
	
	vec3 barvaF;
	if(fract(v * uv.x + 0.025) < .06 || fract(v * uv.y + 0.025) < .06) barvaF = barva_a21;
	else barvaF = barva_b31;
	
	Material ret;
	ret.color = vec4(barvaF, 1.0);
	ret.normal = vec3(0.0, 0.0, 1.0);
	ret.shininess = 30.0;
	ret.diffuseAmt = 1.0;
	ret.specularAmt = 1.3;
	
	return ret;
}
