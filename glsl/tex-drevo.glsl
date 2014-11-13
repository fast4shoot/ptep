#version 330

float PI = 3.14159265358979323846264;
float FREKVENCE = 10.0f;
vec3 drevo = vec3(0.45, 0.227, 0.106);

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	uv *= 16;
	
	float u = uv.x;
	float v = uv.y;
	
	float sum = rand(vec2(u,v)) * 0.3f + 0.7f;
	float svetlo = clamp(pow(cos(u * PI * 10f) * 0.5f + 0.5f, 0.1f) * 1.2, 0f, 1f);
	float svisleCary = (1f - rand(vec2(0, u))) * 0.1f + 1.1f;
	
	float segmentaceu = floor((u + 0.1) * 5) * 0.2;
	float segmentacev = floor(v);
	
	float poziceRozdeleni = segmentacev + rand(vec2(segmentaceu, segmentacev * 17f)) * 0.8 + 0.1;
	float rozdeleni = clamp(pow(cos(clamp((v - poziceRozdeleni + 0.1f) * 10f, 0f, 2f) * PI) * 0.5f + 0.5f, 0.1f) * 1.2, 0f, 1f);
	
	float svislySegment = step(poziceRozdeleni, v);
	float kteryDrevo = svislySegment + segmentacev;
	vec3 modBarvy = vec3(
		rand(vec2(segmentaceu * 654, kteryDrevo * 100.656f)) * 0.5, 
		rand(vec2(segmentaceu * 0.57, kteryDrevo * 941.2f)) * 0.5, 
		rand(vec2(segmentaceu * 9846, kteryDrevo * 80f))
	);
	modBarvy *= 0.2f;
	modBarvy += 1.0f;
	
	vec2 poziceSuku = vec2(segmentaceu + rand(vec2(segmentaceu, 91641 * (svislySegment + 1f))) * 0.08f, segmentacev + rand(vec2(segmentaceu, 400 * (svislySegment + 1f))));
	vec2 rozdilPozic = poziceSuku - uv;
	rozdilPozic.y = rozdilPozic.y * (abs(rozdilPozic.x) * 30);
	float vzdalenostSuku = length(rozdilPozic);
	float velikostSuku = rand(vec2(segmentaceu * 84, svislySegment * 9846)) + 0.5;
	float maskaSuku = (1f - step(0.01 * velikostSuku + rand(uv) * 0.02, vzdalenostSuku)) * step(0.5f, rand(vec2(segmentaceu, svislySegment * 9846)));
	vec3 barvaSuku = vec3(0.5, 0.23, 0.08) * 0.85f;
	
	vec3 barva = sum * svetlo * svisleCary * rozdeleni * modBarvy * mix(drevo, drevo * 0.9, maskaSuku);
	
	return vec4(barva * 1.2, 1.0);
}
