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

enum SectorVisibilityState
{
	SectorVisibilityStateUnknown = 0,
	SectorVisibilityStateVisible = 1,
	SectorVisibilityStateNotVisible = 2
};

/*struct SectorMetrics
{
	Vec3 originOffset;
	int sectorCounts[3];
	int numberOfSectors;
	float sectorSize;
	DynamicLengthArray<Plane> gridPlanes;
};*/

enum SectorCruncherType
{
	SectorCruncherTypeBruteForce/*,
	SectorCruncherTypeOcclusion*/
};

struct Sector
{
	//Vec3 origin;
	AABB aabb;
	//Vec3 insidePoints[SectorMaxInsidePoints];
	//int numberOfInsidePoints;
	//AABB aabb;
	//int visibleSectorIndexesOffset;
	//int numberOfVisibleSectors;
	DynamicLengthArray<Vec3> insidePoints;
	DynamicLengthArray<int> visibleSectorIndexes;
	DynamicLengthArray<int> residentWorldMeshChunkIndexes;
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
	bool isVisibilityOccluder;
};

/*struct CollisionMeshIntersectionResult
{
	Vec3 intersectionPoint;
	Plane intersectionPlane;
	int chunkIndex;
	int faceIndex;
	float distance;
};*/

/*
struct CollisionMeshLineIntersectionDeterminationWorkingData
{
	//DynamicLengthArray<Vec3> gridPlaneIntersections;
	DynamicLengthArray<int> intersectedSectorIndexes;
	//DynamicLengthArray<int> potentiallyIntersectedChunkIndexes;
};*/