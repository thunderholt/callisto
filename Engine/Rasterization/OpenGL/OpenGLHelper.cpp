#include "Rasterization/OpenGL/OpenGLHelper.h"

bool OpenGLHelper::FindAttribute(GLuint programId, GLuint* attributeId, const char* attributeName)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	bool success = false;

	*attributeId = glGetAttribLocation(programId, attributeName);

	if (*attributeId == -1)
	{
		logger->Write("Attribute not found: %s", attributeName);
	}
	else
	{
		success = true;
	}

	return success;
}

bool OpenGLHelper::FindUniform(GLuint programId, GLuint* uniformId, const char* uniformName)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	bool success = false;

	*uniformId = glGetUniformLocation(programId, uniformName);

	if (*uniformId == -1)
	{
		logger->Write("Uniform not found: %s", uniformName);
	}
	else
	{
		success = true;
	}

	return success;
}

bool OpenGLHelper::CreateVertexBufferWithData(GLuint* outBufferId, int dataLengthBytes, const float* data)
{
	glGenBuffers(1, outBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, *outBufferId);
	glBufferData(GL_ARRAY_BUFFER, dataLengthBytes, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true; // TODO - actually check if it was successful.
}

bool OpenGLHelper::CreateIndexBufferWithData(GLuint* outBufferId, int dataLengthBytes, const unsigned short* data)
{
	glGenBuffers(1, outBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *outBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataLengthBytes, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true; // TODO - actually check if it was successful.
}

void OpenGLHelper::SafeDeleteProgram(GLuint* programId)
{
	if (*programId != 0)
	{
		glDeleteProgram(*programId);
		*programId = 0;
	}
}

void OpenGLHelper::SafeDeleteBuffer(GLuint* bufferId)
{
	if (*bufferId != 0)
	{
		glDeleteBuffers(1, bufferId);
		*bufferId = 0;
	}
}

void OpenGLHelper::SafeDeleteTexture(GLuint* textureId)
{
	if (*textureId != 0)
	{
		glDeleteTextures(1, textureId);
		*textureId = 0;
	}
}