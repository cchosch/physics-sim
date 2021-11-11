#pragma once
#include <GL/glew.h>
#include <iostream>

#define ASSERT(x) if(!(x)) __debugbreak()
#define glSafeCall(x) clearErrors(); x; ASSERT(logErrors(#x, __FILE__, __LINE__))

char const* gl_error_string(GLenum const err) noexcept;


void clearErrors() ;
bool logErrors(std::string function, std::string file, int line);
