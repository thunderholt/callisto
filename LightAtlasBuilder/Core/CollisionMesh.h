#pragma once
#include "Include/LightAtlasBuilder.h"

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
	virtual void PushChunk(int startIndex, int numberOfFaces, float* positions, unsigned short* indecies, int lightAtlasIndex, Vec2 lightIslandOffset, Vec2 lightIslandSize);
	virtual void Finish();
	virtual bool DetermineIfLineIntersectsMesh(CollisionLine* line);
	virtual CollisionMeshChunk* GetChunk(int chunkIndex);
	virtual int GetNumberOfChunks();
	virtual CollisionFace* GetFace(int faceIndex);
	virtual int GetNumberOfFaces();

private:
	void BuildGridCells();
	int GetGridCellIndexFromPoint(Vec3* point);
	bool DetermineIfLineIntersectsChunksInGridCell(CollisionLine* line, int gridCellIndex);

	CollisionMeshChunk* chunks;
	int numberOfChunks;
	int nextChunkIndex;
	CollisionFace* faces;
	int numberOfFaces;
	int nextFaceIndex;
	CollisionMeshGridMetrics gridMetrics;
	CollisionMeshGridCell* gridCells;
};