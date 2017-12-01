#pragma once

#include "Include/LightAtlasBuilder.h"

enum WorkerTaskType
{
	WorkerTaskTypeNone,
	WorkerTaskTypeComputeDirectIlluminationForLight
};

class Worker : public IWorker
{
public:
	Worker();
	virtual ~Worker();
	virtual void Init(int startLightIslandIndex, int numberOfLightIslandIndexes);
	virtual void ComputeDirectIlluminationForLightAsync(Light* light);
	virtual bool GetHasFinished();
	virtual void RunThreadEntryPoint();

private:
	void ComputeDirectIlluminationForLightInternal();
	bool FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, int* outFaceIndex, ICollisionMesh* collisionMesh, CollisionMeshChunk* chunk, Vec2 uv);
	//bool FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, ICollisionMesh* collisionMesh, MeshChunkFaceIndex* chunkFaceIndex, Vec2 uv);
	bool CheckIfChunkIsEffectedByLight(CollisionMeshChunk* chunk, Light* light);
	//void FillLightIslandBorders(ILightAtlas* lightAtlas, CollisionMeshChunk* chunk);
	void FillLightIslandBorders(WorldMeshLightIsland* lightIsland);

	WorkerTaskType currentTaskType;
	Light* currentLight;
	bool currentTaskHasFinished;
	int startLightIslandIndex;
	int numberOfLightIslandIndexes;
};
