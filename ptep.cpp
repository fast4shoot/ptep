#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <limits>
#include <type_traits>

#include <GL/glew.h>
#include <GL/gl.h>

#include <SDL2/SDL.h>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "glUtil.h"
#include "readFile.h"
#include "modelLoader.h"
#include "shaderLoader.h"

enum RenderMode
{
	ALL,
	COLOR,
	NORMAL,
	LIGHT
};

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		throw std::runtime_error("Nesprávný počet parametrů. Správné použité je: \nptep <model> <shader s texturou 0> <shader s texturou 1> ...");
	}
	
	auto modelFilename = std::string(argv[1]);
	auto textureFilenames = std::vector<std::string>(argv + 2, argv + argc);
	int currentTextureIdx = 0;
	
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
	
	GLuint textureProgram = 0;
	std::string lastProgramSource;
	
	GLuint tangentSpaceProgram = 0;
	
	unsigned lastTicks = SDL_GetTicks();
	
	bool rotate = true;
	bool showTangent = false;
	bool loadShaders = true;
	RenderMode renderMode = ALL;
	
	auto ticks = SDL_GetTicks();
	auto ticksDelta = 0;
	
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
					switch (event.key.keysym.sym)
					{
						case SDLK_r: rotate = !rotate; break;
						case SDLK_t: showTangent = !showTangent; break;
						case SDLK_a: renderMode = ALL; break;
						case SDLK_s: renderMode = COLOR; break;
						case SDLK_d: renderMode = NORMAL; break;
						case SDLK_f: renderMode = LIGHT; break;
						
						case SDLK_F5: loadShaders = true; break;
						
						case SDLK_LEFT: 
							currentTextureIdx += textureFilenames.size() - 1; currentTextureIdx %= textureFilenames.size();
							std::cout << "Přepínám na texturu " << textureFilenames[currentTextureIdx] << " s idx " << currentTextureIdx << std::endl;
							loadShaders = true;
							break;
						
						case SDLK_RIGHT: 
							currentTextureIdx++; currentTextureIdx %= textureFilenames.size();
							std::cout << "Přepínám na texturu " << textureFilenames[currentTextureIdx] << " s idx " << currentTextureIdx << std::endl;
							loadShaders = false;
							break;
								
					}
					break;
				
				default: 
					break;
			 }
		}
		
		auto lastTicks = ticks;
		ticks = SDL_GetTicks();
		ticksDelta = ticks - lastTicks;
		
		try
		{
			auto programSource = readFile(textureFilenames[currentTextureIdx]);
			if (programSource != lastProgramSource)
			{
				if (textureProgram != 0)
				{
					glDeleteProgram(textureProgram);
					textureProgram = 0;
				}
				
				lastProgramSource = programSource;
				textureProgram = createTextureProgram(programSource);
			}
		
			glCall(glEnable, GL_DEPTH_TEST);	
			glCall(glLineWidth, 2.0f);
			glCall(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
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
		
			// loadneme shadery, pokud je to treba
			if (loadShaders)
			{
				std::cout << "Nahráváme shadery" << std::endl;
				loadShaders = false;
				
				if (textureProgram == 0)					
				{
					glCall(glDeleteProgram, textureProgram);
					textureProgram = 0;
				}
					
				if (tangentSpaceProgram == 0)
				{
					glCall(glDeleteProgram, textureProgram);
					tangentSpaceProgram = 0;
				}
				
				try
				{
					textureProgram = createTextureProgram(readFile(textureFilenames[currentTextureIdx]));
				}
				catch (std::exception& ex)
				{
					std::cerr << ex.what() << std::endl;
				}
				
				try
				{
					std::vector<std::string> vertexShaders;
					std::vector<std::string> geometryShaders;
					std::vector<std::string> fragmentShaders;
					
					vertexShaders.push_back(readFile("glsl/shared/vert.glsl"));
					vertexShaders.push_back(readFile("glsl/tangentSpace/vert.glsl"));
					geometryShaders.push_back(readFile("glsl/tangentSpace/geometry.glsl"));
					fragmentShaders.push_back(readFile("glsl/tangentSpace/fragment.glsl"));
					
					tangentSpaceProgram = createProgram(vertexShaders, geometryShaders, fragmentShaders);
				}
				catch (std::exception& ex)
				{
					std::cerr << ex.what() << std::endl;
				}
			}
		
			std::vector<GLuint> programs = {textureProgram};
			if (showTangent)
			{
				programs.push_back(tangentSpaceProgram);
			}
			
			for (auto program : programs)
			{
				if (program != 0)
				{
					glCall(glUseProgram, program);
				
					auto mvLocation = glCall(glGetUniformLocation, program, "mvMat");
					auto mvNormLocation = glCall(glGetUniformLocation, program, "mvNormMat");
					auto pLocation = glCall(glGetUniformLocation, program, "pMat");
					
					auto mvMat = view * model;
					auto mvNormMat = glm::transpose(glm::inverse(glm::mat3(mvMat)));
					auto pMat = projection;
					
					glCall(glUniformMatrix4fv, mvLocation, 1, GL_FALSE, glm::value_ptr(mvMat));
					glCall(glUniformMatrix3fv, mvNormLocation, 1, GL_FALSE, glm::value_ptr(mvNormMat));
					glCall(glUniformMatrix4fv, pLocation, 1, GL_FALSE, glm::value_ptr(pMat));
				
					auto lightPosLocation = glCall(glGetUniformLocation, program, "lightPos");
					if (lightPosLocation != -1)
					{
						auto lightPos = glm::vec4(200.0f, 200.0f, 0.0f, 1.0f);
						lightPos = view * lightPos;
						glCall(glUniform4fv, lightPosLocation, 1, glm::value_ptr(lightPos));
					}
				
					auto timeLocation = glCall(glGetUniformLocation, program, "time");
					if (timeLocation != 0)
					{
						glCall(glUniform1f, timeLocation, ticks * 0.001);
					}
				
					// modifikatory dle RenderMode
					auto renderColor = glCall(glGetUniformLocation, program, "renderColor");
					auto renderNormal = glCall(glGetUniformLocation, program, "renderNormal");
					auto renderLight = glCall(glGetUniformLocation, program, "renderLight");
					
					if (renderColor != 0)
						glCall(glUniform1i, renderColor, renderMode == COLOR);
				
					if (renderNormal != 0)
						glCall(glUniform1i, renderNormal, renderMode == NORMAL);
						
					if (renderLight != 0)
						glCall(glUniform1i, renderLight, renderMode == LIGHT);
				
					for (int i = 0; i < 4; i++)
						glCall(glEnableVertexAttribArray, i);
						
					// pozice
					glCall(glVertexAttribPointer, 0u, 3, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _x));
					// texcoord
					glCall(glVertexAttribPointer, 1u, 2, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _u)); 
					// normala
					glCall(glVertexAttribPointer, 2u, 3, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _nx));
					// tangent
					glCall(glVertexAttribPointer, 3u, 4, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(Vertex), (void*) offsetof(Vertex, _tanx));
					
					glCall(glDrawElements, GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, nullptr);
					
					for (int i = 0; i < 5; i++)
						glCall(glDisableVertexAttribArray, i);
						
					glCall(glUseProgram, 0);
				}
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
