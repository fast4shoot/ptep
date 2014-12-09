#ifndef GLUTIL_H
#define GLUTIL_H

#include <GL/glew.h>
#include <GL/gl.h>

struct GlErrorVerifier
{
public:
	~GlErrorVerifier();
};

template<typename F, typename... Args>
auto glCall(F f, Args&&... args) -> decltype(f(args...))
{
	auto verifier = GlErrorVerifier();
	return f(args...);
}

const char* glErrorToString(GLenum err);

#endif
