// vystup texturovaci funkce
struct Material
{
	vec4 color;
	vec3 normal;
	float shininess;
	float diffuseAmt;
	float specularAmt;
};

// 2D, 3D a 4D simplex noise
float snoise(vec2 v);
float snoise(vec3 v);
float snoise(vec4 v);

// vysokofrekvencni sum
float rand(vec2 co);

// rotacni matice
mat3 rotationMatrix(vec3 axis, float angle);

// sin a cos s vystupem v rozsahu 0..1
float psin(float x);
float pcos(float x);

// presune hodnotu x z rozsahu -1..1 do rozsahu 0..1
float makePositive(float x);

float permute(float x);
vec3 permute(vec3 x);
vec4 permute(vec4 x);
