#pragma once

#include "Include/FixedLengthArray.h"
#include "Include/DynamicLengthArray.h"
#include "Math/Math.h"

#define AssetMaxFilePathLength 64

typedef void(*ThreadEntryPointFunction)(void*);

struct Buffer
{
	char* data;
	int length;
};

struct AssetRef
{
	char filePath[AssetMaxFilePathLength];
};

//------ Lights -----//

struct LightNode
{
	Vec3 worldPosition;
	Vec3 direction;
	Vec3 invDirection;
	float distance;
	float distanceSqr;
};

struct Light
{
	RgbFloat colour;
	DynamicLengthArray<LightNode> nodes;
	//int numberOfEffectedChunks;
};

//------ Json -----//

enum JsonValueType
{
	JsonValueTypeNull,
	JsonValueTypeNumber,
	JsonValueTypeString,
	JsonValueTypeArray,
	JsonValueTypeObject,
	JsonValueTypeBool
};

enum JsonParserErrorCode
{
	JsonParserErrorCodeNone = 0,
	JsonParserErrorCodeEmptyFile = 1,
	JsonParserErrorCodeInvalidPropertyName = 2,
	JsonParserErrorCodeMalformedProperty = 3,
	JsonParserErrorCodeMalformedString = 4,
	JsonParserErrorCodeMalformedNumber = 5,
	JsonParserErrorCodeMalformedKeyword = 6,
	JsonParserErrorUnrecognisedKeyword = 7
};

//------ Collision Meshes -----//

struct CollisionMeshChunk
{
	int startFaceIndex;
	int numberOfFaces;
	AABB aabb;
	int lightAtlasIndex;
	Vec2 lightIslandOffset;
	Vec2 lightIslandSize;
	//DynamicLengthArray<int> effectiveLightIndexes;
};

struct CollisionChunkFaceIndex
{
	int chunkIndex;
	int faceIndex;
};