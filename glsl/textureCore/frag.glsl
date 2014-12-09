#version 330

uniform vec4 lightPos;

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
	vec3 reflectVec = reflect(-lightVec, modifiedNormal);
	
	vec3 ambient = vec3(1.0);
	vec3 diffuse = vec3(max(0.0, dot(modifiedNormal, lightVec)));
	vec3 specular = vec3(pow(max(dot(reflectVec, eyeVec), 0.0), material.shininess));
	
	vec3 light = ambient * material.ambientAmt + diffuse * material.diffuseAmt + specular * material.specularAmt;
	
	outColor = vec4(material.color.rgb * light, material.color.a);
	//outColor = vec4(normalize(material.normal) * 0.5 + vec3(0.5), material.color.a);
	//outColor = vec4(normNormal * 0.5 + vec3(0.5), material.color.a);
    //outColor = vec4(normTangent * 0.5 + vec3(0.5), material.color.a);
    //outColor = vec4(normBitangent * 0.5 + vec3(0.5), material.color.a);
	//outColor = vec4((cross(varTangent, varNormal) - varBitangent) * 0.5 + vec3(0.5), material.color.a);
}
