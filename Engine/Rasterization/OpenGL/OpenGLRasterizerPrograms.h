#pragma once

#include "Include/Callisto.h"
#include "Rasterization/OpenGL/OpenGL.h"

//-------- World Mesh Chunk --------

struct WorldMeshChunkProgramAttributeLocations
{
	GLuint position;
	GLuint materialTexCoord;
	GLuint lightAtlasTexCoord;
};

struct WorldMeshChunkProgramUniformLocations
{
	GLuint viewProjTransform;
	//GLuint lightIslandOffset;
	//GLuint lightIslandSize;
	GLuint diffuseSampler;
	GLuint lightAtlasSampler;
};

struct WorldMeshChunkProgram
{
	GLuint programId;
	WorldMeshChunkProgramAttributeLocations attributeLocations;
	WorldMeshChunkProgramUniformLocations uniformLocations;
};

//-------- Static Mesh Chunk --------

struct StaticMeshChunkProgramAttributeLocations
{
	GLuint position;
	GLuint materialTexCoord;
};

struct StaticMeshChunkProgramUniformLocations
{
	GLuint worldTransform;
	GLuint viewProjTransform;
	GLuint diffuseSampler;
};

struct StaticMeshChunkProgram
{
	GLuint programId;
	StaticMeshChunkProgramAttributeLocations attributeLocations;
	StaticMeshChunkProgramUniformLocations uniformLocations;
};

//-------- Volume --------

struct VolumeProgramAttributeLocations
{
	GLuint position;
};

struct VolumeProgramUniformLocations
{
	GLuint worldTransform;
	GLuint viewProjTransform;
};

struct VolumeProgram
{
	GLuint programId;
	VolumeProgramAttributeLocations attributeLocations;
	VolumeProgramUniformLocations uniformLocations;
};

//-------- Sprite 3D --------

struct Sprite3DProgramAttributeLocations
{
	GLuint sizeMultiplier;
};

struct Sprite3DProgramUniformLocations
{
	GLuint position;
	GLuint size;
	GLuint centerOfRotation;
	GLuint rotationTransform;
	//GLuint xAxis;
	//GLuint yAxis;
	GLuint spritePosition;
	GLuint spriteSize;
	GLuint spriteSheetSize;
	GLuint spriteSheetSampler;
	GLuint viewTransform;
	GLuint projTransform;
};

struct Sprite3DProgram
{
	GLuint programId;
	Sprite3DProgramAttributeLocations attributeLocations;
	Sprite3DProgramUniformLocations uniformLocations;
};

//-------- Sprite 2D --------

struct Sprite2DProgramAttributeLocations
{
	GLuint sizeMultiplier;
};

struct Sprite2DProgramUniformLocations
{
	GLuint position;
	GLuint size;
	GLuint viewportSize;
	GLuint spritePosition;
	GLuint spriteSize;
	GLuint spriteSheetSize;
	GLuint spriteSheetSampler;
	GLuint tint;
};

struct Sprite2DProgram
{
	GLuint programId;
	Sprite2DProgramAttributeLocations attributeLocations;
	Sprite2DProgramUniformLocations uniformLocations;
};

//-------- Line --------

struct LineProgramAttributeLocations
{
	GLuint position;
};

struct LineProgramUniformLocations
{
	GLuint viewProjTransform;
	GLuint translation;
	GLuint scale;
	GLuint colour;
};

struct LineProgram
{
	GLuint programId;
	LineProgramAttributeLocations attributeLocations;
	LineProgramUniformLocations uniformLocations;
};