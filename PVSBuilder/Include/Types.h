#pragma once

#include "Include/FixedLengthArray.h"
#include "Math/Math.h"

#define SectorMaxInsidePoints 100

typedef void(*ThreadEntryPointFunction)(void*);

struct Buffer
{
	char* data;
	int length;
};

enum SectorVisibilityState
{
	SectorVisibilityStateUnknown = 0,
	SectorVisibilityStateVisible = 1,
	SectorVisibilityStateNotVisible = 2
};

struct SectorMetrics
{
	Vec3 originOffset;
	int sectorCounts[3];
	int numberOfSectors;
	float sectorSize;
};

enum SectorCruncherType
{
	SectorCruncherTypeBruteForce
};

struct Sector
{
	Vec3 origin;
	Vec3 insidePoints[SectorMaxInsidePoints];
	int numberOfInsidePoints;
	int visibleSectorIndexesOffset;
	int numberOfVisibleSectors;
};

//------ Collision Meshes -----//

struct CollisionMeshChunk
{
	int startFaceIndex;
	int numberOfFaces;
};

struct CollisionMeshIntersectionResult
{
	Vec3 intersectionPoint;
	Plane intersectionPlane;
	int chunkIndex;
	int faceIndex;
	float distance;
};