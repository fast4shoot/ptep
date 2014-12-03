#version 330

uniform mat4 mvMat;
uniform mat3 mvNormMat;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

out VertexOutput
{
	vec4 position;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	float tanHandedness;
} output;

void applyPosition(vec4 position);

void main()
{
	output.position = mvMat * inPosition;
	output.texCoord = inTexCoord;
	output.normal = normalize(mvNormMat * inNormal);
	output.tanHandedness = inTangent.w;
	output.tangent = normalize(mvNormMat * inTangent.xyz) * -inTangent.w;
	
	output.bitangent = cross(output.normal, output.tangent) * output.tanHandedness;
	
	applyPosition(output.position);
}
