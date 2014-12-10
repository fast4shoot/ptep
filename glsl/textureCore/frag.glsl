#version 330

uniform vec4 lightPos;
uniform bool renderColor;
uniform bool renderNormal;
uniform bool renderLight;

in VertexOutput
{
	vec4 position;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	float tanHandedness;
} input;

out vec4 outColor;

struct Material
{
	vec4 color;
	vec3 normal;
	float shininess;
	float diffuseAmt;
	float specularAmt;
	float ambientAmt;
};

Material getColorForPoint(vec4 position, vec3 normal, vec2 uv);

void main()
{
	Material material = getColorForPoint(input.position, input.normal, input.texCoord);
	
	vec3 normNormal = normalize(input.normal);
	vec3 normTangent = normalize(input.tangent);
	float normTanHandedness = sign(input.tanHandedness);
	vec3 normBitangent = normalize(input.bitangent);
	mat3 tangentSpace = mat3(normTangent, normBitangent, normNormal);
	vec3 modifiedNormal = tangentSpace * normalize(material.normal);
	
	vec3 eyeVec = normalize(-input.position.xyz);
	vec3 lightVec = normalize(lightPos - input.position).xyz;
	vec3 halfwayVec = normalize(lightVec + eyeVec);
	
	vec3 ambient = vec3(1.0);
	vec3 diffuse = vec3(max(0.0, dot(modifiedNormal, lightVec)));
	vec3 specular = vec3(pow(max(dot(modifiedNormal, halfwayVec), 0.0), material.shininess));
	
	vec3 light = ambient * material.ambientAmt + diffuse * material.diffuseAmt + specular * material.specularAmt;
	
	if (renderColor)
		outColor = material.color;
	else if (renderNormal)
		outColor = vec4(normalize(modifiedNormal) * 0.5 + 0.5, 1.0);
	else if (renderLight)
		outColor = vec4(light * 0.5, 1.0);
	else
		outColor = vec4(material.color.rgb * light, material.color.a);
}
