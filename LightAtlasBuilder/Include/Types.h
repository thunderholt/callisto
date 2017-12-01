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

struct MeshChunkFaceIndex
{
	int chunkIndex;
	int faceIndex;
};

enum LumelState
{
	LumelStateNotSet = 0,
	LumelStateSet = 1,
	LumelStateTempFilled = 2
};

struct Lumel
{
	char state;
	RgbFloat colour;
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

struct LightBlock
{
	LightNode nodes[100];
	int numberOfNodes;
};

struct Light
{
	RgbFloat colour;
	LightBlock blocks[100];
	int numberOfBlocks;
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

//------ World Meshes -----//

struct WorldMeshLightIsland
{
	Vec2i offset;
	Vec2i size;
	int chunkIndex;
	//MeshChunkFaceIndex chunkFaceIndex;
};

//------ Collision Meshes -----//

struct CollisionMeshChunk
{
	int startFaceIndex;
	int numberOfFaces;
	AABB aabb;
	//int lightAtlasIndex;
	//Vec2i lightIslandOffset;
	//Vec2i lightIslandSize;
	//DynamicLengthArray<int> effectiveLightIndexes;
};
