#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstddef>
#include <vector>
#include <fstream>
#include <limits>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
	Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
	{
		_x = x;
		_y = y;
		_z = z;
		_w = 1.0;
		
		_nx = nx;
		_ny = ny;
		_nz = nz;
		
		_u = short(u * (std::numeric_limits<short>::max() + 1));
		_v = short(v * (std::numeric_limits<short>::max() + 1));
	}
	
	float _x;
	float _y;
	float _z;
	float _w;
	
	float _nx;
	float _ny;
	float _nz;
	short _u;
	short _v;
};

static_assert(sizeof(Vertex) == 32, "Vertex nemá velikost 32");

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
	const char* _fName;
	
public:
	GlErrorVerifier(const char* fName)
	{
		_fName = fName;
	}

	~GlErrorVerifier()
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			std::ostringstream stream;
			stream << "OGL error " << glErrorToString(err) << " in " << _fName;
			throw std::runtime_error(stream.str());
		}
	}
};

template<typename F, typename... Args>
auto glCall(const char* fName, F f, Args&&... args) -> decltype(f(args...))
{
	auto verifier = GlErrorVerifier(fName);
	return f(args...);
}

#define GET_FIRST(X, ...) #X
#define GLCALL(...) glCall(GET_FIRST(__VA_ARGS__), __VA_ARGS__)

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
	auto sourcePtr = (GLchar*) source.data();
	auto sourceLength = (GLint) source.size();
	
	auto shader = GLCALL(glCreateShader, type);
	GLCALL(glShaderSource,shader, 1, &sourcePtr, &sourceLength);
	GLCALL(glCompileShader, shader);
	
	GLint compileStatus;
	GLCALL(glGetShaderiv, shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		GLint infoLogLength;
		GLCALL(glGetShaderiv, shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		
		auto log = std::vector<GLchar>(infoLogLength);
		GLCALL(glGetShaderInfoLog, shader, infoLogLength, nullptr, log.data());
			
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
	std::vector<GLuint> shaders;
	try
	{
		shaders.push_back(createShaderFromFile("glsl/vert.glsl", GL_VERTEX_SHADER));		
		shaders.push_back(createShaderFromFile("glsl/noise2D.glsl", GL_FRAGMENT_SHADER));
		shaders.push_back(createShaderFromFile("glsl/noise3D.glsl", GL_FRAGMENT_SHADER));
		shaders.push_back(createShaderFromFile("glsl/noise4D.glsl", GL_FRAGMENT_SHADER));
		shaders.push_back(createShaderFromFile("glsl/fragCore.glsl", GL_FRAGMENT_SHADER));
		shaders.push_back(createShaderFromSource(textureShaderSource, GL_FRAGMENT_SHADER));	
	
		auto program = GLCALL(glCreateProgram);
		for (auto shader : shaders)
			GLCALL(glAttachShader, program, shader);
		
		GLCALL(glLinkProgram, program);
		GLint linkStatus;
		GLCALL(glGetProgramiv, program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus == GL_FALSE)
		{
			GLint infoLogLength;
			GLCALL(glGetProgramiv, program, GL_INFO_LOG_LENGTH, &infoLogLength);
			
			auto log = std::vector<GLchar>(infoLogLength);
			GLCALL(glGetProgramInfoLog, program, infoLogLength, nullptr, log.data());
			std::string exStr = "Linkování programu failnulo: \n";
			exStr.append(log.begin(), log.end());
			throw std::runtime_error(exStr);
		}
		
		for (auto shader : shaders)
			GLCALL(glDeleteShader, shader);
			
		return program;
	}
	catch (std::exception& ex)
	{
		for (auto shader : shaders)
		{
			GLCALL(glDeleteShader, shader);
		}
		throw;
	}
}

void loadModel(const std::string& filename, std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
	assert (vertices.size() == 0);
	assert (indices.size() == 0);
	
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	if (!scene)
	{
		throw std::runtime_error("Nepodařilo se přečíst model");		
	}
	
	if (scene->mNumMeshes != 1)
	{
		throw std::runtime_error("Jsou podporovány pouze modely s počtem meshů = 1");
	}
	
	const aiMesh* mesh = scene->mMeshes[0];
	
	if (!mesh->HasFaces() 
		|| !mesh->HasPositions() 
		|| !mesh->HasNormals() 
		|| !mesh->HasTextureCoords(0) 
		|| mesh->GetNumUVChannels() != 1 )
	{
		throw std::runtime_error("Mesh musí mít facy, pozice, normály a jeden tex coord na vertex");
	}
	
	vertices.reserve(mesh->mNumVertices);
	
	// Prevedeme vertexy
	for (unsigned i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D position = mesh->mVertices[i];
		aiVector3D normal = mesh->mNormals[i];
		aiVector3D uvw = mesh->mTextureCoords[0][i];
		
		vertices.push_back(Vertex(
			position.x, 
			position.y, 
			position.z, 
			normal.x, 
			normal.y, 
			normal.z,
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

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		throw std::runtime_error("Nesprávný počet parametrů. Správné použité je: \nptep <shader s texturou>");
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
	
	std::string modelFilename = "models/teapot.obj";
	std::cout << "Loadujeme model " << modelFilename << std::endl;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	
	loadModel(modelFilename, vertices, indices);
	
	std::cout << "Vytváříme buffery" << std::endl;
	
	GLuint vbo;
	GLuint ibo;
	GLCALL(glGenBuffers, 1, &vbo);
	GLCALL(glGenBuffers, 1, &ibo);

	GLCALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	GLCALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, ibo);
	GLCALL(glBufferData, GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	GLCALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	
	std::cout << "Vstupujeme do hlavní smyčky" << std::endl;
	
	float roty = 0.0f;
	float rotx = 1.0f;
	float dist = -3.0f;
	
	GLuint program = 0;
	std::string lastProgramSource;
	
	auto run = true;
	while (run)
	{
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
							GLCALL(glViewport, 0, 0, windowWidth, windowHeight);
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
					
				default: 
					break;
			 }
		}
		
		try
		{
			auto programSource = readFile(argv[1]);
			if (programSource != lastProgramSource)
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
				GLCALL(glUseProgram, program);
			
				auto mvpLocation = GLCALL(glGetUniformLocation, program, "mvp");
				if (mvpLocation != -1)
				{
					auto projection = glm::perspective(90.0f, float(windowWidth) / float(windowHeight), 0.1f, 100.0f );
					auto translation = glm::translate(projection, glm::vec3(0.0f, 0.0f, dist));
					auto rotation = glm::rotate(
						glm::rotate(
							translation,
							rotx,
							glm::vec3(1.0f, 0.0f, 0.0f)
						),
						roty, glm::vec3(0.0f, 1.0f, 0.0f)
					);
					
					auto mvp = rotation;
					
					GLCALL(glUniformMatrix4fv, mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
				}
			
				GLCALL(glEnableVertexAttribArray, 0);
				GLCALL(glEnableVertexAttribArray, 1);
				GLCALL(glEnableVertexAttribArray, 2);
				GLCALL(glVertexAttribPointer, 0u, 4, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _x));
				GLCALL(glVertexAttribPointer, 1u, 3, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _nx));
				GLCALL(glVertexAttribPointer, 2u, 2, GL_UNSIGNED_SHORT, GL_TRUE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _u)); 
			
				GLCALL(glEnable, GL_DEPTH_TEST);
				
				GLCALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				GLCALL(glDrawElements, GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, nullptr);
				GLCALL(glDisableVertexAttribArray, 0);
				GLCALL(glDisableVertexAttribArray, 1);
				GLCALL(glDisableVertexAttribArray, 2);
				GLCALL(glUseProgram, 0);
			}
			
			SDL_GL_SwapWindow(window);
		}
		catch (std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}
	
	GLCALL(glDeleteBuffers, 1, &vbo);
	GLCALL(glDeleteBuffers, 1, &ibo);
	SDL_GL_DeleteContext(glCtx);
	SDL_DestroyWindow(window);
	SDL_Quit();
}