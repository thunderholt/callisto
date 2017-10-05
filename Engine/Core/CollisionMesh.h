#pragma once
#include "Include/Callisto.h"

class CollisionMesh : public ICollisionMesh
{
public:
	CollisionMesh();
	virtual ~CollisionMesh();
	virtual void Allocate(int numberOfChunks, int numberOfFaces);
	virtual void PushChunk(int startIndex, int numberOfFaces, int materialAssetIndex, float* positions, unsigned short* indecies);
	virtual bool FindNearestRayIntersection(CollisionMeshIntersectionResult* out, Ray3* ray);
	virtual bool FindNearestLineIntersection(CollisionMeshIntersectionResult* out, CollisionLine* line);
	virtual bool FindNearestSphereCollision(CollisionMeshIntersectionResult* out, Sphere* sphere, Vec3* movementDirection);
	virtual CollisionMeshChunk* GetChunk(int chunkIndex);
	virtual int GetNumberOfChunks();
	virtual CollisionFace* GetFace(int faceIndex);
	virtual int GetNumberOfFaces();

private:
	CollisionMeshChunk* chunks;
	int numberOfChunks;
	int nextChunkIndex;
	CollisionFace* faces;
	int numberOfFaces;
	int nextFaceIndex;
};