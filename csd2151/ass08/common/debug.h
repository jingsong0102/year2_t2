#pragma once

#include <GL\glew.h>
  
int checkForOpenGLError(const char*, int);

void debugCallback( GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar * msg, const void * param );

 void dumpGLInfo(bool dumpExtensions = false);
