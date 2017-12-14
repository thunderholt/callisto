#pragma once

#include "Include/FixedLengthArray.h"
#include "Include/DynamicLengthArray.h"
#include "Math/Math.h"

#define AssetMaxFilePathLength 64

typedef void(*ThreadEntryPointFunction)(void*);

struct Config
{
	int indirectIlluminationHemispehereCircleCount;
	//int indirectIlluminationHemispehereSegmentCount;
};

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

//------ Lumels -----//

enum LumelState
{
	LumelStateNotSet = 0,
	LumelStateSet = 1,
	LumelStateTempFilled = 2
};

struct LumelBounceTarget
{
	int lumelIndex;
};

struct Lumel
{
	bool isParticipant;
	char state;
	Vec3 worldPosition;
	Vec3 normal;
	int chunkIndex;
	int faceIndex;
	RgbFloat colour;
	float directIlluminationIntensityCache;
	float directIlluminationAverageDistanceToLightSqrCache;
	float indirectIlluminationIntensitySampleCache;
	//int numberOfIndirectIlluminationIntensitySamples;
	LumelBounceTarget bounceTarget;
	int numberOfBounceSamples;
};

//------ Lights -----//

struct LightNode
{
	Vec3 worldPosition;
	Vec3 direction;
	Vec3 invDirection;
};

struct LightBlock
{
	LightNode nodes[32];
	int numberOfNodes;
};

struct Light
{
	int ownerChunkIndex;
	RgbFloat colour;
	LightBlock blocks[100];
	int numberOfBlocks;
	float minConeAngle;
	float distance;
	float distanceSqr;
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

//------ Materials -----//

struct MaterialStaticLightingDetails
{
	bool emitsLight;
	RgbFloat colour;
	float power;
	Vec2i gridDimensions;
	float minConeAngle;
	float distance;
};

//------ Collision Meshes -----//

struct CollisionMeshChunk
{
	int startFaceIndex;
	int numberOfFaces;
	AABB aabb;
	MaterialStaticLightingDetails staticLightingDetails;
	//int lightAtlasIndex;
	//Vec2i lightIslandOffset;
	//Vec2i lightIslandSize;
	//DynamicLengthArray<int> effectiveLightIndexes;
};
