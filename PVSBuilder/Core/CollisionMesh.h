#pragma once
#include "Include/PVSBuilder.h"

class CollisionMesh : public ICollisionMesh
{
public:
	CollisionMesh();
	virtual ~CollisionMesh();
	virtual void Allocate(int numberOfChunks, int numberOfFaces);
	virtual void PushChunk(int startIndex, int numberOfFaces, float* positions, unsigned short* indecies);
	virtual void Finish();
	virtual bool DetermineIfPointIsInsideIndoorMesh(Vec3* point);
	//virtual bool FindNearestRayIntersection(CollisionMeshIntersectionResult* out, Ray3* ray);
	//virtual bool FindNearestLineIntersection(CollisionMeshIntersectionResult* out, CollisionLine* line);
	virtual bool DetermineIfLineIntersectsMesh(CollisionLine* line, SectorMetrics* sectorMetrics, Sector* sectors, CollisionMeshLineIntersectionDeterminationWorkingData* workingData);
	virtual CollisionMeshChunk* GetChunk(int chunkIndex);
	virtual int GetNumberOfChunks();
	virtual CollisionFace* GetFace(int faceIndex);
	virtual int GetNumberOfFaces();

private:
	void FindPointCompletelyOutsideOfExtremities();

	CollisionMeshChunk* chunks;
	int numberOfChunks;
	int nextChunkIndex;
	CollisionFace* faces;
	int numberOfFaces;
	int nextFaceIndex;
	Vec3 pointCompletelyOutsideOfExtremities;
};