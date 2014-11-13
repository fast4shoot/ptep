#version 330

float PI = 3.14159265358979323846264;
float FREKVENCE = 10.0f;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	uv *= 16;
	
	float u = uv.x;
	float v = uv.y;

	float ucol = 
		clamp(sin(u * 10.0f) * 10.0f + 0.5f, 0.0f, 1.0f);
	float vcol = 
		clamp(sin(v * 10.0f) * 0.5f + 0.5f, 0.0f, 1.0f);
		
	float vsegmentace = 
		clamp(sin(v * 10.0f) * 10f + 0.5f, 0f, 1f);
	float usegmentace = 
		clamp(sin(u * 10.0f) * 10f + 0.5f, 0f, 1f);
	float segmentace = abs(usegmentace - vsegmentace);
	

	float carkyMaska = 
		clamp(step(0.75, sin(v * 20.0f - PI * 0.5)), 0.0f, 1.0f);
	
	vec3 carky = vec3(1.0f, 0.2f, 0.2f);
	
	float carkySvetlo = pow(abs(vcol * 2f - 1.0), 16.0f);
	
	carkyMaska *= 1f - segmentace;
	carky *= carkySvetlo;

	

	float gray = abs(ucol - vcol) * 0.8 + 0.2;
	
	gray *= vec3(rand(uv)) * 0.2 + 0.8;	
	vec3 barva = mix(carky, gray * vec3(0.6f, 0.7f, 1.0f), 1 - carkyMaska);
	

	return vec4(
		barva,
		1.0f);
}
