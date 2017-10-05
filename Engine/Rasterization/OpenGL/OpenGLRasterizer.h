#pragma once

#include "Include/Callisto.h"
#include "Rasterization/OpenGL/OpenGL.h"
#include "Rasterization/OpenGL/OpenGLRasterizerPrograms.h"

struct RasterizerPrograms
{
	WorldMeshChunkProgram worldMeshChunk;
	StaticMeshChunkProgram staticMeshChunk;
	VolumeProgram volume;
	Sprite3DProgram sprite3D;
	Sprite2DProgram sprite2D;
	LineProgram line;
};

struct Sprite3DBuffers
{
	GLuint sizeMultiplier;
};

struct Sprite2DBuffers
{
	GLuint sizeMultiplier;
};

struct LineCubeBuffers
{
	GLuint position;
};

struct LineSphereBuffers
{
	GLuint position;
};

class OpenGLRasterizer : public IRasterizer
{
public:
	OpenGLRasterizer();
	virtual ~OpenGLRasterizer();

	virtual bool Init();
	virtual bool PostCoreAssetLoadInit();
	//virtual void CleanUp();
	virtual void RasterizeFrame(RasterJob* rasterJob);
	virtual ProviderId CreateVertexBuffer(int sizeBytes, const float* data);
	virtual ProviderId CreateIndexBuffer(int sizeBytes, const unsigned short* data);
	virtual void DeleteVertexBuffer(ProviderId* id);
	virtual void DeleteIndexBuffer(ProviderId* id);
	virtual ProviderId CreateTexture(int width, int height, RgbaUByte* imageData);
	virtual void DeleteTexture(ProviderId* id);
	virtual Vec2* GetViewportSize();

private:
	//bool Init();
	void DrawWorldMeshChunks(RasterJob* rasterJob);
	void DrawStaticMeshes(RasterJob* rasterJob);
	void DrawVolumes(RasterJob* rasterJob);
	void DrawSprites3D(RasterJob* rasterJob);
	void DrawSprites2D(RasterJob* rasterJob);
	void DrawLineSpheres(RasterJob* rasterJob);
	void DrawLineCubes(RasterJob* rasterJob);
	void DrawLineCube(CommonRastorizationParameters* commonRasterizationParameters, Vec3* position, Vec3* size, RgbFloat* unoccludedColour, RgbFloat* occludedColour);
	void DrawLineSphere(CommonRastorizationParameters* commonRasterizationParameters, Vec3* position, float radius, RgbFloat* unoccludedColour, RgbFloat* occludedColour);
	bool LoadPrograms();
	bool LoadWorldMeshChunkProgram();
	bool LoadStaticMeshChunkProgram();
	bool LoadVolumeProgram();
	bool LoadSprite3DProgram();
	bool LoadSprite2DProgram();
	bool LoadLineProgram();
	bool CreateProgram(GLuint* outProgram, const char* vertexShaderSource, const char* fragmentShaderSource);
	bool CreateShader(GLuint* outShader, GLenum shaderType, const char* sourceCode);
	bool LinkProgram(GLuint* outProgram, const GLuint vertexShader, const GLuint fragmentShader);
	bool CreateSprite3DBuffers();
	bool CreateSprite2DBuffers();
	bool CreateLineCubeBuffers();
	bool CreateLineSphereBuffers();
	bool LoadViewportSize();

	//bool isInitialised;
	RasterizerPrograms programs;
	Sprite3DBuffers sprite3DBuffers;
	Sprite2DBuffers sprite2DBuffers;
	LineCubeBuffers lineCubeBuffers;
	LineSphereBuffers lineSphereBuffers;
	Vec2 viewportSize;
};