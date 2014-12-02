#version 330

uniform mat4 mvMat;
uniform mat4 pMat;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;


out vec4 varPosition;
out vec2 varTexCoord;
out vec3 varNormal;
out vec3 varTangent;
out float varTanHandedness;

void main()
{
	varPosition = mvMat * inPosition;
	varTexCoord = inTexCoord;
	varNormal = (mvMat * vec4(inNormal, 0.0)).xyz;
	varTangent = (mvMat * vec4(inTangent.xyz, 0.0)).xyz;
	varTanHandedness = inTangent.w;
	
	
	gl_Position = pMat * varPosition;
}
