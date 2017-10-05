#pragma once

#include "Include/Callisto.h"
#include "Rasterization/OpenGL/OpenGL.h"

class OpenGLHelper
{
public:
	static bool FindAttribute(GLuint programId, GLuint* attributeId, const char* attributeName);
	static bool FindUniform(GLuint programId, GLuint* uniformId, const char* uniformName);
	static bool CreateVertexBufferWithData(GLuint* outBufferId, int dataLengthBytes, const float* data);
	static bool CreateIndexBufferWithData(GLuint* outBufferId, int dataLengthBytes, const unsigned short* data);
	static void SafeDeleteProgram(GLuint* programId);
	static void SafeDeleteBuffer(GLuint* bufferId);
	static void SafeDeleteTexture(GLuint* textureId);
};
