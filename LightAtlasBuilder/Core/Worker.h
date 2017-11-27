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
	virtual void Init(int startMeshChunkIndex, int meshChunkIndexesStride);
	virtual void ComputeDirectIlluminationForLightAsync(Light* light);
	virtual bool GetHasFinished();
	virtual void RunThreadEntryPoint();

private:
	void ComputeDirectIlluminationForLightInternal();
	bool FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, ICollisionMesh* collisionMesh, CollisionMeshChunk* chunk, Vec2 uv);
	bool CheckIfChunkIsEffectedByLight(CollisionMeshChunk* chunk, Light* light);

	WorkerTaskType currentTaskType;
	Light* currentLight;
	bool currentTaskHasFinished;
	int startMeshChunkIndex;
	int meshChunkIndexesStride;
};
