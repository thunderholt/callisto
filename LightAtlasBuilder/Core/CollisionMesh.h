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
	virtual void PushChunk(int startIndex, int numberOfFaces, Vec3* positions, Vec3* normals, Vec2* uvs, unsigned short* indecies);
	virtual void Finish();
	virtual bool DetermineIfLineIntersectsMesh(CollisionLine* line, int ignoreChunkIndex);
	virtual bool FindNearestLineIntersectWithMesh(Vec3* outIntersection, MeshChunkFaceIndex* outChunkFaceIndex, CollisionLine* line, int ignoreChunkIndex);
	virtual CollisionMeshChunk* GetChunk(int chunkIndex);
	virtual int GetNumberOfChunks();
	virtual CollisionFace* GetFace(int faceIndex);
	virtual int GetNumberOfFaces();

private:
	void BuildGridCells();
	int GetGridCellIndexFromPoint(Vec3* point);
	bool DetermineIfLineIntersectsChunksInGridCell(CollisionLine* line, int gridCellIndex, int ignoreChunkIndex);
	bool FindNearestLineIntersectionWithChunksInGridCell(Vec3* outIntersection, MeshChunkFaceIndex* outChunkFaceIndex, CollisionLine* line, int gridCellIndex, int ignoreChunkIndex);

	CollisionMeshChunk* chunks;
	int numberOfChunks;
	int nextChunkIndex;
	CollisionFace* faces;
	int numberOfFaces;
	int nextFaceIndex;
	CollisionMeshGridMetrics gridMetrics;
	CollisionMeshGridCell* gridCells;
};