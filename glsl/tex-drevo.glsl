struct Material
{
	vec4 color;
	vec3 normal;
	float shininess;
	float diffuseAmt;
	float specularAmt;
};

float PI = 3.14159265358979323846264;
float FREKVENCE = 10.0f;
vec3 drevo = vec3(0.45, 0.227, 0.106);

mat3 rotationMatrix(vec3 axis, float angle)
{
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;
	return mat3(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
		oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
		oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c);
}

float generujUhelNormaly(float x)
{
	return -(pow(abs(cos(x * PI * 0.5)), 0.5) - 1) * sign(sin(x * PI));
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

Material getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	uv *= 12;
	
	float u = uv.x;
	float v = uv.y;
	
	float svetlo = clamp(pow(cos(u * PI * FREKVENCE) * 0.5f + 0.5f, 0.1f) * 1.2, 0f, 1f);
	float svisleCary = (1f - rand(vec2(0, u))) * 0.1f + 1.1f;
	
	float segmentaceu = floor((u + 0.1) * 5) * 0.2;
	float segmentacev = floor(v);
	
	float poziceRozdeleni = segmentacev + rand(vec2(segmentaceu, segmentacev * 17f)) * 0.8 + 0.1;
	
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
	
	vec3 barva = svisleCary * modBarvy * mix(drevo, drevo * 0.9, maskaSuku);
	
	float horizontalniNormalaUhel = generujUhelNormaly(u * FREKVENCE);
	horizontalniNormalaUhel += snoise(uv * vec2(50.0, 0.1) + vec2(0.0, kteryDrevo * 15.0)) * 0.25;
	vec3 normala = vec3(0.0, 0.0, 1.0);
	normala = rotationMatrix(vec3(0.0, -1.0, 0.0), clamp(horizontalniNormalaUhel * 0.4, -0.5, 0.5) * PI) * normala;
	
	//float vertikalniNormalaUhel = generujUhelNormaly(clamp((v - poziceRozdeleni + 0.1f) * 10f, 0f, 2f) * 0.5 - 0.5);
	float vertikalniNormalaUhel = generujUhelNormaly(clamp((v - poziceRozdeleni) * 16 + 1.0, 0.0, 2.0));
	normala = rotationMatrix(vec3(-1.0, 0.0, 0.0), clamp(vertikalniNormalaUhel * 0.4, -0.5, 0.5) * PI) * normala;
	
	
	Material ret;
	ret.color = vec4(barva * 1.2, 1.0);//  * abs(vertikalniNormalaUhel);
	ret.normal = normala;
	ret.shininess = 3.0;
	ret.diffuseAmt = 1.0;
	ret.specularAmt = 0.9;
	
	return ret;
}
