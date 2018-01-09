#pragma once

#include "Include/LightAtlasBuilder.h"

enum WorkerTaskType
{
	WorkerTaskTypeNone,
	WorkerTaskTypeComputeLumelBasicData,
	WorkerTaskTypeComputeLumelBounceTargets,
	WorkerTaskTypeComputeDirectIlluminationIntensitiesForCurrentLight,
	WorkerTaskTypeComputeIndirectIlluminationBounceIntensitiesForCurrentLight,
	//WorkerTaskTypeAccumulateIndirectIlluminationIntensitiesForCurrentLight,
	//WorkerTaskTypeAverageIndirectIlluminationIntensitiesForCurrentLight,
	//WorkerTaskTypeCompositeColourForCurrentLight,
	WorkerTaskTypeFillBorders
};

class Worker : public IWorker
{
public:
	Worker();
	virtual ~Worker();
	//virtual void Init(int startLightIslandIndex, int numberOfLightIslandIndexes);
	virtual void SetCurrentLightAtlasDetails(int* lightIslandIndexes, int numberofLightIslandIndexes, BitField* effectedChunksFieldForCurrentLightAtlas);
	virtual void SetCurrentLight(ExpandedLight* light, BitField* effectedChunksFieldForCurrentLight);
	virtual void ComputeLumelBasicDataAsync();
	virtual void ComputeLumelBounceTargetsAsync();
	virtual void ComputeDirectIlluminationIntensitiesForCurrentLightAsync();
	virtual void ComputeIndirectIlluminationBounceIntensitiesForCurrentLightAsync(int bounceNumber);
	//virtual void AccumulateIndirectIlluminationIntensitiesForCurrentLightAsync(NormalWithinHemisphereCalculationMetrics* normalWithinHemisphereCalculationMetrics, int hemisphereCircleIndex, int hemisphereSegmentIndex, int bounceNumber);
	//virtual void AverageIndirectIlluminationIntensitiesForCurrentLightAsync(int bounceNumber);
	//virtual void CompositeColourForCurrentLightAsync();
	virtual void FillBordersAsync();
	virtual bool GetHasFinished();
	virtual void RunThreadEntryPoint();
	//virtual void DumpStats();
	//virtual int GetTotalFailures();

private:
	void ComputeLumelBasicDataInternal();
	void ComputeLumelBounceTargetsInternal();
	void ComputeDirectIlluminationIntensitiesForCurrentLightInternal();
	void ComputeIndirectIlluminationBounceIntensitiesForCurrentLightInternal();
	//void StartNewIndirectIlluminationBounce();
	//void AccumulateIndirectIlluminationIntensitiesForCurrentLightInternal();
	//void AverageIndirectIlluminationIntensitiesForCurrentLightInternal();
	//void CompositeColourForCurrentLightInternal();
	void FillBordersInternal();

	bool FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, int* outFaceIndex, ICollisionMesh* collisionMesh, CollisionMeshChunk* chunk, Vec2 uv);
	//bool FindWorldPositionForLightIslandTexel(Vec3* outWorldPosition, Vec3* outNormal, ICollisionMesh* collisionMesh, MeshChunkFaceIndex* chunkFaceIndex, Vec2 uv);
	//bool CheckIfChunkIsEffectedByLight(CollisionMeshChunk* chunk, Light* light);
	//void FillLightIslandBorders(ILightAtlas* lightAtlas, CollisionMeshChunk* chunk);
	void FillLightIslandBorders(WorldMeshLightIsland* lightIsland);
	Vec2 CalculatePixelCentre();
	void FindBounceTarget(int* outBounceTargetLumelIndex, bool* outSkyIsVisible, Lumel* lumels, ICollisionMesh* collisionMesh, CollisionLine* line, WorldMeshLightIsland* lightIsland, Vec2i* lightAtlasSize);
	int FindParticipantLumelInVicinity(Lumel* lumels, int x, int y, WorldMeshLightIsland* lightIsland, Vec2i* lightAtlasSize);
	//float RecurseIndirectIlluminationBounces(Lumel* lumels, Lumel* lumel, float baseDistanceToLightSqr, int recursionDepth);

	WorkerTaskType currentTaskType;
	ExpandedLight* currentLight;
	BitField* effectedChunksFieldForCurrentLightAtlas;
	BitField* effectedChunksFieldForCurrentLight;
	//NormalWithinHemisphereCalculationMetrics* currentNormalWithinHemisphereCalculationMetrics;
	//int currentHemisphereCircleIndex;
	//int currentHemisphereSegmentIndex;
	int currentBounceNumber;
	bool currentTaskHasFinished;
	//int startLightIslandIndex;
	int* lightIslandIndexes;
	int numberOfLightIslandIndexes;

	/*int failureType1Count;
	int failureType2Count;
	int failureType3Count;
	int failureType4Count;
	int failureType5Count;*/
};
