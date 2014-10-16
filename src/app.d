import derelict.opengl3.gl3;
import derelict.sdl2.sdl;
import std.conv;

align(16) struct Vertex
{
	this(float x, float y, float z, float r, float g, float b)
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
}

immutable data = [
	Vertex(-1, -1, 0, 0, 1, 1),
	Vertex(1, -1, 0, 1, 1, 0),
	Vertex(-1, 1, 0, 1, 0, 1),
	Vertex(1, -1, 0, 0, 1, 1),
	Vertex(1, 1, 0, 1, 1, 0),
	Vertex(-1, 1, 0, 1, 0, 1),
];

auto glCall(alias F, Params...)(Params params)
{
	scope(exit)
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			import std.stdio : stderr;
			stderr.writefln("%s: %x", F.stringof, err);
		}
	}
	
	return F(params);
}

GLuint createProgram(string vertName, string fragName)
{
	GLuint createShader(string filename, GLenum type)
	{
		import std.file : read;
		auto source = filename.read;
		auto sourcePtr = cast(char*) source.ptr;
		auto sourceLength = source.length.to!GLint;
		auto shader = glCall!glCreateShader(type);
		glCall!glShaderSource(shader, 1, &sourcePtr, &sourceLength);
		glCall!glCompileShader(shader);
		
		GLint compileStatus;
		glCall!glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus == GL_FALSE)
		{
			GLint infoLogLength;
			glCall!glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
			
			auto log = new GLchar[infoLogLength];
			glCall!glGetShaderInfoLog(shader, infoLogLength, null, log.ptr);
			throw new Exception("Kompilace shaderu " ~ text ~ " failnula: \n" ~ log.to!string);
		}
		
		return shader;
	}
	
	GLuint[] shaders;
	scope(failure)
	{
		foreach (shader; shaders)
		{
			import std.stdio;
			debug writeln("Mazu shader ", shader);
			glDeleteShader(shader);
		}
	}
	
	shaders ~= createShader(vertName, GL_VERTEX_SHADER);
	shaders ~= createShader(fragName, GL_FRAGMENT_SHADER);

	auto program = glCall!glCreateProgram();
	foreach (shader; shaders)
		glCall!glAttachShader(program, shader);
	
	glCall!glLinkProgram(program);
	
	foreach (shader; shaders)
		glCall!glDeleteShader(shader);
		
	return program;
}

void main()
{
	DerelictGL3.load();
	DerelictSDL2.load();
	
	SDL_Init(SDL_INIT_VIDEO);
	scope(exit) SDL_Quit();
	
	auto window = SDL_CreateWindow(
		"SDL2 OpenGL Example", 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		768, 768, 
		SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
		
	auto glCtx = SDL_GL_CreateContext(window);
	
	DerelictGL3.reload();
	
	GLuint vbo;
	glCall!glGenBuffers(1, &vbo);
	scope(exit) glDeleteBuffers(1, &vbo);
	glCall!glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glCall!glBufferData(GL_ARRAY_BUFFER, data.length * Vertex.sizeof, data.ptr, GL_STATIC_DRAW);
	
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
							glViewport(0, 0, event.window.data1, event.window.data2);
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
			scope(exit) glDeleteProgram(program);
		
			glCall!glUseProgram(program);
			glCall!glEnableVertexAttribArray(0);
			glCall!glEnableVertexAttribArray(1);
			glCall!glVertexAttribPointer(0u, 4, GL_FLOAT, GL_FALSE, Vertex.sizeof.to!GLsizei, cast(void*) Vertex._x.offsetof);
			glCall!glVertexAttribPointer(1u, 3, GL_FLOAT, GL_FALSE, Vertex.sizeof.to!GLsizei, cast(void*) Vertex._r.offsetof);
			
			glCall!glClear(GL_COLOR_BUFFER_BIT);
			glCall!glDrawArrays(GL_TRIANGLES, 0, data.length.to!GLsizei);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glUseProgram(0);
			
			SDL_GL_SwapWindow(window);
		}
		catch (Exception ex)
		{
			import std.stdio : stderr;
			stderr.writeln(ex);
		}
	}
	
	SDL_GL_DeleteContext(glCtx);
	SDL_DestroyWindow(window);
	
	
	
}
