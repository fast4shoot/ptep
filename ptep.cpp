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

struct Vertex
{
	Vertex(float x, float y, float z, float r, float g, float b)
	{
		_x = x;
		_y = y;
		_z = z;
		_w = 1;
		_r = r;
		_g = g;
		_b = b;
	}
	
	float _x, _y, _z, _w;
	float _r, _g, _b;
	float _padding1;
};

std::vector<Vertex> data = {
	Vertex(-1, -1, 0, 0, 1, 1),
	Vertex(1, -1, 0, 1, 1, 0),
	Vertex(-1, 1, 0, 1, 0, 1),
	Vertex(1, -1, 0, 0, 1, 1),
	Vertex(1, 1, 0, 1, 1, 0),
	Vertex(-1, 1, 0, 1, 0, 1),
};

struct GlErrorVerifier
{
	~GlErrorVerifier()
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			std::ostringstream stream;
			stream << "OGL error " << err;
			throw std::runtime_error(stream.str());
		}
	}
};

template<typename F, typename... Args>
auto glCall(F f, Args&&... args) -> decltype(f(args...))
{
	GlErrorVerifier verifier;
	return f(args...);
}

GLuint createShader(const std::string& filename, GLenum type)
{
	std::ifstream t(filename.c_str());
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string data = buffer.str();
	auto sourcePtr = (GLchar*) data.data();
	auto sourceLength = (GLint) data.size();
	
	auto shader = glCall(glCreateShader, type);
	glCall(glShaderSource, shader, 1, &sourcePtr, &sourceLength);
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

GLuint createProgram(const std::string& vertName, const std::string& fragName)
{
	std::vector<GLuint> shaders;
	try
	{
		shaders.push_back(createShader(vertName, GL_VERTEX_SHADER));
		shaders.push_back(createShader(fragName, GL_FRAGMENT_SHADER));	
	
		auto program = glCall(glCreateProgram);
		for (auto shader : shaders)
			glCall(glAttachShader, program, shader);
		
		glCall(glLinkProgram, program);
		
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

int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	
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
	
	GLuint vbo;
	glCall(glGenBuffers, 1, &vbo);
	glCall(glDeleteBuffers, 1, &vbo);

	glCall(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	glCall(glBufferData, GL_ARRAY_BUFFER, data.size() * sizeof(Vertex), data.data(), GL_STATIC_DRAW);
	
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
							glCall(glViewport, 0, 0, event.window.data1, event.window.data2);
							break;
						default:
							break;
					}
					break;
					
					
				default: 
					break;
			 }
		}
		
		try
		{
			auto program = createProgram("vert", "frag");
		
			glCall(glUseProgram, program);
			glCall(glEnableVertexAttribArray, 0);
			glCall(glEnableVertexAttribArray, 1);
			glCall(glVertexAttribPointer, 0u, 4, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _x));
			glCall(glVertexAttribPointer, 1u, 3, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _r));
			
			glCall(glClear, GL_COLOR_BUFFER_BIT);
			glCall(glDrawArrays, GL_TRIANGLES, 0, (GLsizei) data.size());
			glCall(glDisableVertexAttribArray, 0);
			glCall(glDisableVertexAttribArray, 1);
			glCall(glUseProgram, 0);
			
			glCall(glDeleteProgram, program);
			SDL_GL_SwapWindow(window);
		}
		catch (std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}
	
	glCall(glDeleteBuffers, 1, &vbo);
	SDL_GL_DeleteContext(glCtx);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
