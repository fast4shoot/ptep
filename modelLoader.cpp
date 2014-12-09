#include "modelLoader.h"

#include <cassert>
#include <stdexcept>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

#include <glm/glm.hpp>

std::ostream& operator<<(std::ostream& stream, const aiVector3D& vec)
{
	stream << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return stream;
}

glm::vec3 aiToGlm(const aiVector3D& vector)
{
	return glm::vec3(vector.x, vector.y, vector.z);
}

Vertex::Vertex(
	float x, float y, float z, 
	float nx, float ny, float nz, 
	float tanx, float tany, float tanz, float tanHandedness,
	float u, float v):
	_x(x),
	_y(y),
	_z(z),
	
	_nx(nx),
	_ny(ny),
	_nz(nz),
	
	_tanx(tanx),
	_tany(tany),
	_tanz(tanz),
	_tanHandedness(tanHandedness),
	
	_u(u),
	_v(v)
{}

void loadModel(
	const std::string& filename, 
	std::vector<Vertex>& vertices, 
	std::vector<GLuint>& indices)
{
	assert (vertices.size() == 0);
	assert (indices.size() == 0);
	
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace );
	if (!scene)
	{
		throw std::runtime_error("Nepodařilo se přečíst model");		
	}
	
	if (scene->mNumMeshes != 1)
	{
		throw std::runtime_error("Jsou podporovány pouze modely s počtem meshů = 1");
	}
	
	const aiMesh* mesh = scene->mMeshes[0];
	
	if (!mesh->HasFaces())
		throw std::runtime_error("Mesh musí mít facy");
		
	if (!mesh->HasPositions())
		throw std::runtime_error("Mesh musí mít pozice");
		
	if (!mesh->HasNormals())
		throw std::runtime_error("Mesh musí mít normály");
		
	if (!mesh->HasTextureCoords(0))
		throw std::runtime_error("Mesh musí mít tex coordy");
		
	if (mesh->GetNumUVChannels() != 1)
	{
		std::stringstream ss;
		ss << "Mesh musí mít jeden UV kanál, ne " << mesh->GetNumUVChannels() << " kanálů";
		throw std::runtime_error(ss.str());
	}
	
	vertices.reserve(mesh->mNumVertices);
	
	// Prevedeme vertexy
	for (unsigned i = 0; i < mesh->mNumVertices; i++)
	{
		auto position = mesh->mVertices[i];
		auto normal = aiToGlm(mesh->mNormals[i]);
		auto tangent = aiToGlm(mesh->mTangents[i]);
		auto bitangent = aiToGlm(mesh->mBitangents[i]);
		auto uvw = mesh->mTextureCoords[0][i];
		
		tangent = glm::normalize(tangent - normal * glm::dot(normal, tangent));
		auto tanHandedness = glm::dot(glm::cross(normal, tangent), bitangent) < 0.0f ? -1.0f : 1.0f;
		
		vertices.push_back(Vertex(
			position.x, 
			position.y, 
			position.z, 
			normal.x, 
			normal.y, 
			normal.z,
			tangent.x,
			tangent.y,
			tangent.z,
			tanHandedness,
			uvw.x,
			uvw.y));
	}
	
	indices.reserve(mesh->mNumFaces * 3);
	
	// Prevedeme indexy
	for (unsigned i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace face = mesh->mFaces[i];
		
		if (face.mNumIndices != 3)
		{
			throw std::runtime_error("Jsou podporovány pouze facy se třemi body");
		}
		
		for (unsigned j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
}
