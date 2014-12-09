#include "glUtil.h"

#include <sstream>
#include <stdexcept>

GlErrorVerifier::~GlErrorVerifier()
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::ostringstream stream;
		stream << "OGL error " << glErrorToString(err);
		throw std::runtime_error(stream.str());
	}
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
