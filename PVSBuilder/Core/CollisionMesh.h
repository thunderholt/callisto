#pragma once
#include "Include/PVSBuilder.h"

struct CollisionMeshGridMetrics
{
	Vec3 gridOrigin;
	Vec3i gridDimensions;
	float gridCellSize;
	int numberOfCells;
	DynamicLengthArray<Plane> gridPlanes;
};

struct CollisionMeshGridCell
{
	DynamicLengthArray<int> residentChunkIndexes;
};

class CollisionMesh : public ICollisionMesh
{
public:
	CollisionMesh();
	virtual ~CollisionMesh();
	virtual void AllocateGeometry(int numberOfChunks, int numberOfFaces);
	virtual void AllocateGrid(Vec3 gridOrigin, Vec3i gridDimensions, float gridCellSize);
	virtual void PushChunk(int startIndex, int numberOfFaces, float* positions, unsigned short* indecies, bool isVisibilityOccluder);
	virtual void Finish();
	virtual bool DetermineIfPointIsInsideIndoorMesh(Vec3* point);
	virtual bool DetermineIfPointIsPotentiallyInsideOutdoorMesh(Vec3* point);
	virtual bool DetermineIfLineIntersectsMesh(CollisionLine* line, bool excludeNonOccludingChunks);
	virtual CollisionMeshChunk* GetChunk(int chunkIndex);
	virtual int GetNumberOfChunks();
	virtual CollisionFace* GetFace(int faceIndex);
	virtual int GetNumberOfFaces();

private:
	void FindPointCompletelyOutsideOfExtremities();
	void CalculateBoundingSphereDiameter();
	void BuildGridCells();
	int GetGridCellIndexFromPoint(Vec3* point);
	bool DetermineIfLineIntersectsChunksInGridCell(CollisionLine* line, int gridCellIndex, bool excludeNonOccludingChunks);

	CollisionMeshChunk* chunks;
	int numberOfChunks;
	int nextChunkIndex;
	CollisionFace* faces;
	int numberOfFaces;
	int nextFaceIndex;
	Vec3 pointCompletelyOutsideOfExtremities;
	float boundingSphereDiameter;
	CollisionMeshGridMetrics gridMetrics;
	CollisionMeshGridCell* gridCells;
};