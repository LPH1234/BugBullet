#pragma once

#include<glad/glad.h>
#include <string>
using namespace std;

#ifndef SOIL2_H
#define SOIL2_H
#include <SOIL2/SOIL2.h>
#endif

#include "../Utils/Utils.h"

unsigned int TextureFromFile(const char *path, const string &directory);
void loadTexture(char const* path, unsigned int* textureID);
void loadTextureRGBA(char const* path, unsigned int* textureID);
GLuint loadCubeMapTexture(std::vector<std::string> picFilePathVec,
	GLint internalFormat = GL_RGB,
	GLenum picFormat = GL_RGB,
	GLenum picDataType = GL_UNSIGNED_BYTE,
	int loadChannels = SOIL_LOAD_RGB);