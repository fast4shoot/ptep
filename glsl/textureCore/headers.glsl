// vystup texturovaci funkce
struct Material
{
	vec4 color;
	vec3 normal;
	float shininess;
	float diffuseAmt;
	float specularAmt;
	float ambientAmt;
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

// linearni verze smoothstepu
float linearsmoothstep(float edge0, float edge1, float x);
vec2 linearsmoothstep(float edge0, float edge1, vec2 x);
vec3 linearsmoothstep(float edge0, float edge1, vec3 x);
vec4 linearsmoothstep(float edge0, float edge1, vec4 x);
vec2 linearsmoothstep(vec2 edge0, vec2 edge1, vec2 x);
vec3 linearsmoothstep(vec3 edge0, vec3 edge1, vec3 x);
vec4 linearsmoothstep(vec4 edge0, vec4 edge1, vec4 x);

float permute(float x);
vec3 permute(vec3 x);
vec4 permute(vec4 x);
