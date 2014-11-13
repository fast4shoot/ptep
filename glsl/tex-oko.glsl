#version 330

float PI = 3.14159265358979323846264;
float FREKVENCE = 10.0f;
vec3 drevo = vec3(0.45, 0.227, 0.106);

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(float x, float y)
{
	return rand(vec2(x, y));
}

float psin(float x)
{
	return sin(x) * 0.5 + 0.5;
}

float KONEC_COCKY = 0.3;
float KONEC_DUHOVKY = 0.7;
float OKRAJ = 0.01;

vec4 getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{	
	uv = uv - vec2(0.5, 0.5);

	float dist = length(uv) * 2;
	
	vec3 barvaCocky = vec3(0.1, 0.1, 0.13) * (rand(uv) * 0.2 + 0.8);
	vec3 barvaDuhovky = vec3(0.3, 0.5, 0.8) * (rand(uv) * 0.2 + 0.8);
	vec3 barvaBelima = vec3(0.9, 0.9, 0.9); 
	
	float polar = fract((PI + atan(uv.x, uv.y)) / (2 * PI));
	float okrajCocky = rand(polar * 5, 20) * OKRAJ;
	okrajCocky = OKRAJ * psin(polar * PI * 100);
	
	vec3 cockaDuhovka = mix(
		barvaCocky, 
		barvaDuhovky, 
		smoothstep(KONEC_COCKY - okrajCocky - OKRAJ, KONEC_COCKY - okrajCocky + OKRAJ, dist));
		
	vec3 barva = mix(
		cockaDuhovka,
		barvaBelima,
		smoothstep(KONEC_DUHOVKY - OKRAJ, KONEC_DUHOVKY + OKRAJ, dist));  
	
	return vec4(barva, 1.0f);
}
