#ifndef LOAD_TGA_H
#define LOAD_TGA_H
#include <GL\glew.h>

GLuint LoadTGA(const char *file_path, bool repeat=true);
GLuint Load2DTGA(const char *file_path, bool repeat=false, bool mipmap=true);

#endif