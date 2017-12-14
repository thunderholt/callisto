#pragma once

#include "Include/LightAtlasBuilder.h"

enum WorkerTaskType
{
	WorkerTaskTypeNone,
	WorkerTaskTypeComputeBasicLumelData,
	WorkerTaskTypeComputeDirectIlluminationIntensitiesForCurrentLight,
	WorkerTaskTypeComputeIndirectIlluminationBouncesTargetsForCurrentLight,
	WorkerTaskTypeAccumulateIndirectIlluminationIntensitiesForCurrentLight,
	WorkerTaskTypeCompositeColourForCurrentLight,
	WorkerTaskTypeFillBorders
};

class Worker : public IWorker
{
public:
	Worker();
	virtual ~Worker();
	virtual void Init(int startLightIslandIndex, int numberOfLightIslandIndexes);
	virtual void SetCurrentLight(Light* light);
	virtual void ComputeBasicLumelDataAsync();
	virtual void ComputeDirectIlluminationIntensitiesForCurrentLightAsync();
	virtual void ComputeIndirectIlluminationBouncesTargetsForCurrentLightAsync(NormalWithinHemisphereCalculationMetrics* normalWithinHemisphereCalculationMetrics, int hemisphereCircleIndex, int hemisphereSegmentIndex);
	virtual void AccumulateIndirectIlluminationIntensitiesForCurrentLightAsync();
	virtual void CompositeColourForCurrentLightAsync();
	virtual void FillBordersAsync();
	virtual bool GetHasFinished();
	virtual void RunThreadEntryPoint();
	virtual void DumpStats();
	virtual int GetTotalFailures();

private:
	void ComputeBasicLumelDataInternal();
	void ComputeDirectIlluminationIntensitiesForCurrentLightInternal();
	void ComputeIndirectIlluminationBouncesTargetsForCurrentLightInternal();
	void AccumulateIndirectIlluminationIntensitiesForCurrentLightInternal();
	void CompositeColourForCurrentLightInternal();
	void FillBordersInternal();

	bool FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, int* outFaceIndex, ICollisionMesh* collisionMesh, CollisionMeshChunk* chunk, Vec2 uv);
	//bool FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, ICollisionMesh* collisionMesh, MeshChunkFaceIndex* chunkFaceIndex, Vec2 uv);
	bool CheckIfChunkIsEffectedByLight(CollisionMeshChunk* chunk, Light* light);
	//void FillLightIslandBorders(ILightAtlas* lightAtlas, CollisionMeshChunk* chunk);
	void FillLightIslandBorders(WorldMeshLightIsland* lightIsland);
	float RecurseIndirectIlluminationBounces(Lumel* lumels, Lumel* lumel, float baseDistanceToLightSqr, int recursionDepth);

	WorkerTaskType currentTaskType;
	Light* currentLight;
	NormalWithinHemisphereCalculationMetrics* currentNormalWithinHemisphereCalculationMetrics;
	int currentHemisphereCircleIndex;
	int currentHemisphereSegmentIndex;
	bool currentTaskHasFinished;
	int startLightIslandIndex;
	int numberOfLightIslandIndexes;

	int failureType1Count;
	int failureType2Count;
	int failureType3Count;
	int failureType4Count;
	int failureType5Count;
};
