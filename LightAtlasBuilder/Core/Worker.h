#pragma once

#include "Include/LightAtlasBuilder.h"

enum WorkerTaskType
{
	WorkerTaskTypeNone,
	WorkerTaskTypeComputeLightIslands
};

class Worker : public IWorker
{
public:
	Worker();
	virtual ~Worker();
	virtual void Init(int startMeshChunkIndex, int meshChunkIndexesStride);
	virtual void ComputeLightIslandsAsync();
	virtual bool GetHasFinished();
	virtual void RunThreadEntryPoint();

private:
	void ComputeLightIslandsInternal();
	bool FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, ICollisionMesh* collisionMesh, CollisionMeshChunk* chunk, Vec2 uv);
	
	WorkerTaskType currentTaskType;
	bool currentTaskHasFinished;
	int startMeshChunkIndex;
	int meshChunkIndexesStride;
};
