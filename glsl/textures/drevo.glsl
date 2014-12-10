float PI = 3.14159265358979323846264;
float FREKVENCE = 2f;
vec3 drevo = vec3(0.45, 0.227, 0.106);

float generujUhelNormaly(float x)
{
	return -(pow(abs(cos(x * PI * 0.5)), 0.5) - 1) * sign(sin(x * PI));
}

float lokalniRozdeleniSegmentu(float segu, float segv)
{
	return rand(vec2(segu, segv * 17f)) * 0.8 + 0.1;
}

float globalniRozdeleniSegmentu(float segu, float segv)
{
	return segv + lokalniRozdeleniSegmentu(segu, segv);
}

Material getColorForPoint(vec4 position, vec3 normal, vec2 uv)
{
	uv.s *= 60;
	uv.t *= 12;
	
	float u = uv.x;
	float v = uv.y;
	
	float segmentaceu = floor(u);
	float segmentacev = floor(v);
	
	float globalniSpodek;
	float globalniVrsek;
	
	float rozdeleniSegmentu = globalniRozdeleniSegmentu(segmentaceu, segmentacev);
	
	if (v > rozdeleniSegmentu)
	{
		globalniSpodek = rozdeleniSegmentu;
		globalniVrsek = globalniRozdeleniSegmentu(segmentaceu, segmentacev + 1);
	}
	else
	{
		globalniSpodek = globalniRozdeleniSegmentu(segmentaceu, segmentacev - 1);
		globalniVrsek = rozdeleniSegmentu;
	}
	
	float lokalniu = u - segmentaceu;
	float lokalniv = v - globalniSpodek;
	vec2 lokalniPozice = vec2(lokalniu, lokalniv);
	
	float vyska = globalniVrsek - globalniSpodek;
	
	float lokalniunorm = lokalniu;
	float lokalnivnorm = lokalniv / vyska;
	
	float globalniudreva = segmentaceu;
	float globalnivdreva = globalniSpodek;
	
	vec3 modBarvy = vec3(
		rand(vec2(segmentaceu * 654, globalniSpodek * 100.656f)) * 0.5, 
		rand(vec2(segmentaceu * 0.57, globalniSpodek * 941.2f)) * 0.5, 
		rand(vec2(segmentaceu * 9846, globalniSpodek * 80f))
	);
	modBarvy *= 0.2f;
	modBarvy += 1.0f;
	
	float lokalniXSuku = rand(vec2(globalniudreva, globalnivdreva) * 548) * 0.5 + 0.1;
	float globalniXSuku = lokalniXSuku + globalniudreva;
	float lokalniYSuku = rand(vec2(globalniudreva, globalnivdreva) * 86768) * 0.8 + 0.1;
	vec2 lokalniPoziceSuku = vec2(lokalniXSuku, lokalniYSuku);
	vec2 vektorKSuku = lokalniPoziceSuku - lokalniPozice;
	vektorKSuku.y = vektorKSuku.y * (abs(vektorKSuku.x) * 30);
	float modifikovanaVzdalenostSuku = length(vektorKSuku);

	float velikostSuku = rand(vec2(globalniudreva * 84, globalnivdreva * 9846)) * 5;
	float existenceSuku = step(0.5f, rand(vec2(globalniudreva, globalnivdreva * 9846)));
	float maskaSuku = (1f - step(0.01 * velikostSuku + rand(uv) * 0.02, modifikovanaVzdalenostSuku)) * existenceSuku;
	
	vec3 barva = modBarvy * mix(drevo, drevo * 0.85, maskaSuku);
	
	float horizontalniNormalaUhel = generujUhelNormaly((u + 0.5) * FREKVENCE);
	float vzdalenostSuku = distance(lokalniPozice, lokalniPoziceSuku);
	float modVzdalenostiSuku = (1.0 - smoothstep(0.0, velikostSuku * 0.08, vzdalenostSuku));
	horizontalniNormalaUhel += snoise(vec2(mix(u, globalniXSuku, existenceSuku * modVzdalenostiSuku), v) * vec2(15.0, 0.1) + vec2(0.0, globalniSpodek * 15.0)) * 0.25;
	vec3 normala = vec3(0.0, 0.0, 1.0);
	normala = rotationMatrix(vec3(0.0, -1.0, 0.0), clamp(horizontalniNormalaUhel * 0.4, -0.5, 0.5) * PI) * normala;
	
	float generatorVertikalniNormaly = lokalnivnorm < 0.5 ? lokalniv : -vyska + lokalniv;
	float vertikalniNormalaUhel = generujUhelNormaly(clamp((generatorVertikalniNormaly * 1) * 16 + 1.0, 0.0, 2.0));
	normala = rotationMatrix(vec3(-1.0, 0.0, 0.0), clamp(vertikalniNormalaUhel * 0.3, -0.5, 0.5) * PI) * normala;
	
	
	Material ret;
	ret.color = vec4(barva * 1.2, 1.0);// * (lokalniunorm);
	ret.normal = normala;
	//ret.normal = vec3(0.0, 0.0, 1.0);
	ret.shininess = 20.0;
	ret.diffuseAmt = 1.0;
	ret.specularAmt = 0.9;
	ret.ambientAmt = 0.3;
	
	return ret;
}
