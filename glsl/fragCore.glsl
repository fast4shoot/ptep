#version 330

uniform vec4 lightPos;

in vec4 varPosition;
in vec2 varTexCoord;
in vec3 varNormal;
in vec3 varTangent;
in vec3 varBitangent;

out vec4 outColor;

struct Material
{
	vec4 color;
	vec3 normal;
	float shininess;
	float diffuseAmt;
	float specularAmt;
};

Material getColorForPoint(vec4 position, vec3 normal, vec2 uv);

void main()
{
	Material material = getColorForPoint(varPosition, varNormal, varTexCoord);
	
	mat3 tangentSpace = mat3(normalize(varTangent), normalize(varBitangent), normalize(varNormal));
	vec3 modifiedNormal = tangentSpace * normalize(material.normal);
	
	vec3 eyeVec = normalize(-varPosition.xyz);
	vec3 lightVec = normalize(lightPos - varPosition).xyz;
	vec3 reflectVec = reflect(-lightVec, modifiedNormal);
	
	vec3 ambient = vec3(0.3);
	vec3 diffuse = vec3(max(0.0, dot(modifiedNormal, lightVec)));
	vec3 specular = vec3(pow(max(dot(reflectVec, eyeVec), 0.0), material.shininess));
	
	vec3 light = ambient + diffuse * material.diffuseAmt + specular * material.specularAmt;
	
	outColor = vec4(material.color.rgb * light, material.color.a);
	//outColor = vec4(modifiedNormal * 0.5 + vec3(0.5), material.color.a);
}
