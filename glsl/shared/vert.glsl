#version 330

uniform mat4 mvMat;
uniform mat3 mvNormMat;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

out VertexOUT
{
	vec4 position;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	float tanHandedness;
} OUT;

void applyPosition(vec4 position);

void main()
{
	OUT.position = mvMat * inPosition;
	OUT.texCoord = inTexCoord;
	OUT.normal = normalize(mat3(mvNormMat) * inNormal);
	OUT.tanHandedness = inTangent.w;
	OUT.tangent = normalize(mat3(mvNormMat) * inTangent.xyz) * -inTangent.w;
	
	OUT.bitangent = cross(OUT.normal, OUT.tangent) * OUT.tanHandedness;
	
	applyPosition(OUT.position);
}
