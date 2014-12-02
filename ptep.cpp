#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <fstream>
#include <limits>
#include <type_traits>

#include <GL/glew.h>
#include <GL/gl.h>

#include <SDL2/SDL.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

std::ostream& operator<<(std::ostream& stream, const aiVector3D& vec)
{
	stream << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return stream;
}

struct Vertex
{
	Vertex(
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

glm::vec3 aiToGlm(const aiVector3D& vector)
{
	return glm::vec3(vector.x, vector.y, vector.z);
}

const char* glErrorToString(GLenum err)
{
	switch (err)
	{
		case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default: throw std::runtime_error("Invalid GL error code");
	}
}

struct GlErrorVerifier
{
private:
	
public:
	GlErrorVerifier()
	{}

	~GlErrorVerifier()
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			std::ostringstream stream;
			stream << "OGL error " << glErrorToString(err);
			throw std::runtime_error(stream.str());
		}
	}
};

template<typename F, typename... Args>
auto glCall(F f, Args&&... args) -> decltype(f(args...))
{
	auto verifier = GlErrorVerifier();
	return f(args...);
}

std::string readFile(const std::string& filename)
{
	std::ifstream t(filename);
	
	if (!t)
	{
		throw std::runtime_error("Nelze otevřít shader " + filename);
	}
	
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

GLuint createShaderFromSource(const std::string& source, GLenum type, const std::string& filename = "")
{
	auto sourcePtr = (const GLchar*) source.data();
	auto sourceLength = (GLint) source.size();
	
	auto shader = glCall(glCreateShader, type);
	glCall(glShaderSource,shader, 1, &sourcePtr, &sourceLength);
	glCall(glCompileShader, shader);
	
	GLint compileStatus;
	glCall(glGetShaderiv, shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glCall(glGetShaderiv, shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		
		auto log = std::vector<GLchar>(infoLogLength);
		glCall(glGetShaderInfoLog, shader, infoLogLength, nullptr, log.data());
			
		auto exStr = "Kompilace shaderu " + filename + " failnula: \n";
		exStr.append(log.begin(), log.end());
		
		throw std::runtime_error(exStr);
	}
	
	return shader;
}

GLuint createShaderFromFile(const std::string& filename, GLenum type)
{
	return createShaderFromSource(readFile(filename), type, filename);
}

GLuint createProgram(const std::string& textureShaderSource)
{
	static std::string headersSource = readFile("glsl/headers.glsl");
	
	auto modifiedTextureShaderSource = "#version 330\n" + headersSource + "#line 1\n" + textureShaderSource;
	
	std::vector<GLuint> shaders;
	try
	{
		shaders.push_back(createShaderFromFile("glsl/vert.glsl", GL_VERTEX_SHADER));		
		shaders.push_back(createShaderFromFile("glsl/noise2D.glsl", GL_FRAGMENT_SHADER));
		shaders.push_back(createShaderFromFile("glsl/noise3D.glsl", GL_FRAGMENT_SHADER));
		shaders.push_back(createShaderFromFile("glsl/noise4D.glsl", GL_FRAGMENT_SHADER));
		shaders.push_back(createShaderFromFile("glsl/noiseFuncs.glsl", GL_FRAGMENT_SHADER));
		shaders.push_back(createShaderFromFile("glsl/fragCore.glsl", GL_FRAGMENT_SHADER));
		shaders.push_back(createShaderFromSource(modifiedTextureShaderSource, GL_FRAGMENT_SHADER));	
	
		auto program = glCall(glCreateProgram);
		for (auto shader : shaders)
			glCall(glAttachShader, program, shader);
		
		glCall(glLinkProgram, program);
		GLint linkStatus;
		glCall(glGetProgramiv, program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus == GL_FALSE)
		{
			GLint infoLogLength;
			glCall(glGetProgramiv, program, GL_INFO_LOG_LENGTH, &infoLogLength);
			
			auto log = std::vector<GLchar>(infoLogLength);
			glCall(glGetProgramInfoLog, program, infoLogLength, nullptr, log.data());
			std::string exStr = "Linkování programu failnulo: \n";
			exStr.append(log.begin(), log.end());
			throw std::runtime_error(exStr);
		}
		
		for (auto shader : shaders)
			glCall(glDeleteShader, shader);
			
		return program;
	}
	catch (std::exception& ex)
	{
		for (auto shader : shaders)
		{
			glCall(glDeleteShader, shader);
		}
		throw;
	}
}

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
	
	//prevedeme vertexy vektoru (normaly, tangenty, bitangenty)
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		throw std::runtime_error("Nesprávný počet parametrů. Správné použité je: \nptep <shader s texturou> <model>");
	}
	
	SDL_Init(SDL_INIT_VIDEO);
	
	int windowWidth = 1024;
	int windowHeight = 768; 
	
	auto window = SDL_CreateWindow(
		"PTEP", 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		768, 768, 
		SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
		
	auto glCtx = SDL_GL_CreateContext(window);
	
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// glew nejede
		std::ostringstream stream;
		stream << "GLEW error: " << glewGetErrorString(err);
		throw std::runtime_error(stream.str());
	}
	std::cout << "Jedeme: " << glewGetString(GLEW_VERSION) << std::endl;
	
	std::string modelFilename = argv[2];
	std::cout << "Loadujeme model " << modelFilename << std::endl;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	
	loadModel(modelFilename, vertices, indices);
	
	std::cout << "Vytváříme buffery" << std::endl;
	
	GLuint vbo;
	GLuint ibo;
	glCall(glGenBuffers, 1, &vbo);
	glCall(glGenBuffers, 1, &ibo);

	glCall(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	glCall(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, ibo);
	glCall(glBufferData, GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glCall(glBufferData, GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	
	std::cout << "Vstupujeme do hlavní smyčky" << std::endl;
	
	float modelRoty = 0.0f;
	float roty = 0.0f;
	float rotx = 1.0f;
	float dist = -3.0f;
	
	GLuint program = 0;
	std::string lastProgramSource;
	
	unsigned lastTicks = SDL_GetTicks();
	
	bool rotate = true;
	
	auto run = true;
	while (run)
	{
		auto ticks = SDL_GetTicks();
		modelRoty += rotate ? 0.0002 * (ticks - lastTicks) : 0.0;
		lastTicks = ticks;
		
		SDL_Event event;
		while (SDL_PollEvent(&event)) 
		{
			 switch (event.type)
			 {
				 case SDL_QUIT:
					run = false;
					break;
					
				case SDL_WINDOWEVENT:
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_RESIZED:
							windowWidth = event.window.data1;
							windowHeight = event.window.data2;
							glCall(glViewport, 0, 0, windowWidth, windowHeight);
							break;
						default:
							break;
					}
					break;
					
				case SDL_MOUSEMOTION:
					if (event.motion.state & SDL_BUTTON_LMASK)
					{
						roty += event.motion.xrel * 0.01f;
						rotx += event.motion.yrel * 0.01f;
					}
					
					if (event.motion.state & SDL_BUTTON_RMASK)
					{
						
					}
					break;
					
				case SDL_MOUSEWHEEL:
					dist += event.wheel.y * 0.1f;
					break;
					
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_r)
						rotate = !rotate;
					
					break;
				
				default: 
					break;
			 }
		}
		
		try
		{
			auto programSource = readFile(argv[1]);
			if (programSource != lastProgramSource || true)
			{
				if (program != 0)
				{
					glDeleteProgram(program);
					program = 0;
				}
				
				lastProgramSource = programSource;
				program = createProgram(programSource);
			}
		
			if (program != 0)
			{
				glCall(glUseProgram, program);
			
				auto projection = glm::perspective(90.0f, float(windowWidth) / float(windowHeight), 0.1f, 100.0f );
				auto view = glm::rotate(
					glm::rotate(
						glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist)),
						rotx,
						glm::vec3(1.0f, 0.0f, 0.0f)
					),
					roty, glm::vec3(0.0f, 1.0f, 0.0f)
				);
				auto model = glm::rotate(
					glm::mat4(1.0f),
					modelRoty, glm::vec3(0.0f, 1.0f, 0.0f)
				);
			
				auto mvLocation = glCall(glGetUniformLocation, program, "mvMat");
				auto pLocation = glCall(glGetUniformLocation, program, "pMat");
				
				auto mvMat = view * model;
				auto pMat = projection;
				
				glCall(glUniformMatrix4fv, mvLocation, 1, GL_FALSE, glm::value_ptr(mvMat));
				glCall(glUniformMatrix4fv, pLocation, 1, GL_FALSE, glm::value_ptr(pMat));
			
				auto lightPosLocation = glCall(glGetUniformLocation, program, "lightPos");
				if (lightPosLocation != -1)
				{
					auto lightPos = glm::vec4(20.0f, 20.0f, 20.0f, 1.0f);
					lightPos = view * lightPos;
					glCall(glUniform4fv, lightPosLocation, 1, glm::value_ptr(lightPos));
				}
			
				for (int i = 0; i < 5; i++)
					glCall(glEnableVertexAttribArray, i);
					
				// pozice
				glCall(glVertexAttribPointer, 0u, 3, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _x));
				// texcoord
				glCall(glVertexAttribPointer, 1u, 2, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _u)); 
				// normala
				glCall(glVertexAttribPointer, 2u, 3, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _nx));
				// tangent
				glCall(glVertexAttribPointer, 3u, 4, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _tanx));
			
				glCall(glEnable, GL_DEPTH_TEST);
				glCall(glEnable, GL_CULL_FACE);
				glCall(glCullFace, GL_BACK);
				
				glCall(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glCall(glDrawElements, GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, nullptr);
				
				for (int i = 0; i < 5; i++)
					glCall(glDisableVertexAttribArray, i);
					
				glCall(glUseProgram, 0);
			}
			
			SDL_GL_SwapWindow(window);
		}
		catch (std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}
	
	glCall(glDeleteBuffers, 1, &vbo);
	glCall(glDeleteBuffers, 1, &ibo);
	SDL_GL_DeleteContext(glCtx);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
