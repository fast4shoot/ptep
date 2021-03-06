#version 330

layout(triangles) in;
layout(line_strip, max_vertices = 18) out;

uniform mat4 pMat;

in VertexOUT
{
	vec4 position;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	float tanHandedness;
} IN[];

out GeometryToFragment
{
	vec3 color;
} OUT;

vec3 vecToColor(vec3 x)
{
	return x * 0.5 + vec3(0.5);
}

vec4 transformPosition(vec3 x)
{
	return pMat * vec4(x, 1.0);
}


void main()
{
	for (int i = 0; i < 3; i++)
    {
		vec4 position = gl_in[i].gl_Position;
		vec4 normal = vec4(IN[i].normal, 0.0) * 0.1;
		vec4 tangent = vec4(IN[i].tangent, 0.0) * 0.1;
		vec4 bitangent = vec4(IN[i].bitangent, 0.0) * 0.1;
		
        gl_Position = pMat * position;
        OUT.color = vecToColor(IN[i].normal);
        EmitVertex();
        
        gl_Position = pMat * (position + normal);
        EmitVertex();
        EndPrimitive();
        
        gl_Position = pMat * position;
        OUT.color = vecToColor(IN[i].tangent);
        EmitVertex();
        
        gl_Position = pMat * (position + tangent);
        EmitVertex();
        EndPrimitive();
        
        gl_Position = pMat * position;
        OUT.color = vecToColor(IN[i].bitangent);
        EmitVertex();
        
        gl_Position += pMat * (position + bitangent);
        EmitVertex();
        EndPrimitive();
    }
}
