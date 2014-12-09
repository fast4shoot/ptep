#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>

struct Vertex
{
	Vertex(
		float x, float y, float z, 
		float nx, float ny, float nz, 
		float tanx, float tany, float tanz, float tanHandedness,
		float u, float v);

	float _x;
	float _y;
	float _z;
	float _pad0;
	
	float _u;
	float _v;
	float _pad1;
	float _pad2;
	
	float _nx;
	float _ny;
	float _nz;
	float _pad3;
	
	float _tanx;
	float _tany;
	float _tanz;
	float _tanHandedness;
};

static_assert(sizeof(Vertex) == 64, "Vertex nemá velikost 64B");


void loadModel(
	const std::string& filename, 
	std::vector<Vertex>& vertices, 
	std::vector<GLuint>& indices);

#endif
