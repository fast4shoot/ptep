#define SHARP_NOISE 0
 
/*float totallyRandomNoise(vec2 a) {
  return (float)((rand(a) % 1000)/1000.0);
}*/
 
float perlinNoise(float x, float y) {
	
	float alpha = 2.0, beta = 2.0, sum = 0.0, scale = 1.0, val;
	int n = 12, i;
	vec2 p = vec2(x, y); 
	
	for(i = 0; i < n; i++) {
		
		val = snoise(p);
		sum += val / scale;
		scale *= alpha;
		p = p * beta;
	}
	return sum;
}
 
// Hodnoty periody zakladneho sinusu, urcuju uhol sklonu
float xPerioda = 5.0;
float yPerioda = 10.0;
 
// Hodnoty turbulencie - urcujuce tvar zakrivenia
float turbulVelkost = 32.0;
float turbulNasobic = 50.0;
 
// NEW: Delitele, hodnoty bude mozno nutne delit uz pocas vypoctu nastavenim na hodnotu x > 1
float divX = 1.0;
float divY = 1.0;
 
// NEW: "Magicke" konstanty, pre vytvaranie zakrivenia
float magic1 = 128.0;
float magic2 = 256.0;
float magic3 = 32.0;
float magic4 = 2.0;
 
// Konstanta deliaca celkovy obsah, aby bolo vobec nieco vidiet
float totalDiv = (1.0 / 2000.0);
 
vec3 kamen = vec3(0.78, 0.75, 0.58);
float MOJE_PI = 3.1415926535;

vec2 cells(vec2 input, out vec2 vektor) {
	
		vec2 P = floor(input);
		vec2 desCast = fract(input);
		
		vec4 desCastX = desCast.x + vec4(-0.5,-1.5,-0.5,-1.5);
		vec4 desCastY = desCast.y + vec4(-0.5,-0.5,-1.5,-1.5);
		
		vec4 perx = permute(P.x + vec4(0.0,1.0,0.0,1.0));
		vec4 pery = permute((P.y) + vec4(0.0,0.0,1.0,1.0));
		
		vec4 oX = mod(pery, 6.0) * (1.0/7.0) + (0.5/7.0);
		vec4 oY = mod(floor(perx * (1.0/50.0)), .0) * (1.0/7.0) + (0.5/7.0);
		
		vec4 dX = desCastX + 1.2 * oX;
		vec4 dY = desCastY + 0.8 * oY;
	
		vektor = vec2(dX.z, dY.y);
		
		vec4 d = dX * dX + dY * dY;
		
		int j = 0;
		for (int i = 0; i < 4; i++)
		{
			if (d[j] > d[i])
			{
				j = i;
			}
		}
		
		vektor = vec2(dX[j], dY[j]);
		
		d.xy = min(d.xy, d.zw);
		d.x = min(d.x, d.y);
	
		return vec2(d.x, d.y);
}
 
float fbmGen(vec2 uv)
{
	return snoise(vec3(2.0  * uv, 0.0)) +   
       (1.0/2.0)  * snoise(vec3(10.0 * uv, 2.0)) +
       (1.0/4.0)  * snoise(vec3(20.0 * uv, 4.0)) +
	   (1.0/8.0)  * snoise(vec3(40.0 * uv, 6.0)) +
       (1.0/16.0) * snoise(vec3(80.0 * uv, 8.0));
}
 
Material getColorForPoint(vec4 position, vec3 normal, vec2 uv) {
       
        uv *= 6; // magic3;
        float u = uv.x;
        float v = uv.y;
        
        Material retmat;
        retmat.shininess = 30.0; // odrazivost
        retmat.diffuseAmt = 1.0; // difuzna odrazivost
        // spekularna odr.
        
       float fbm = fbmGen(uv);
       
       float frb = abs(sin(fbm + 1.0 * MOJE_PI)) * 1.25;
       
       vec3 farba = vec3(0.4 * vec3(frb) + 0.5);
       vec3 normala = vec3(0.0,0.0,1.0);
       
		float mult = 10.0;
		vec2 testVektor;
		vec2 res = cells(uv * mult, testVektor);
		float color = 1.0 - 1.7 * res.x;
		vec3 bledomodra = vec3(0.7, 0.8, 1.0);
		
		float normalaSum = makePositive(fbmGen(uv * 5));
		res.x += max(normalaSum * 0.5, 0);
		
		vec2 rotTestVektor = vec2(testVektor.x, -testVektor.y);
		mat3 rotNormaly = rotationMatrix(vec3(rotTestVektor, 0.0), clamp(sqrt(res.x) * MOJE_PI * 0.5, 0, 0.3 * MOJE_PI));
		
		retmat.color = vec4(farba * color * bledomodra, 1.0);
		retmat.normal = rotNormaly * vec3(0.0,0.0,1.0);
		retmat.specularAmt = 0.9 * (fbm + 1) * 0.7;
		return retmat;
 }
