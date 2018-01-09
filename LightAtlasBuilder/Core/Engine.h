#pragma once

#include "Include/LightAtlasBuilder.h"

class Engine : public IEngine
{
public:
	Engine();
	virtual ~Engine();
	virtual void BuildLightAtlases(const char* worldMeshAssetFilePath, const char* assetsFolderPath);
	virtual ILogger* GetLogger();
	virtual IWorldMeshAsset* GetWorldMeshAsset();
	virtual ILightAtlas* GetCurrentLightAtlas();
	//virtual Light* GetLight(int index);
	//virtual int GetNumberOfLights();
	virtual IRayTracer* GetRayTracer();
	virtual IThreadManager* GetThreadManager();
	virtual ITimestampProvider* GetTimestampProvider();
	virtual const char* GetAssetsFolderPath();
	virtual const Config* GetConfig();

private:
	//void InitWorkers();
	//void InitLightAtlas();
	//void InitLights();
	void ComputeLightIslandsOnWorkers(int lightAtlasIndex);
	//void FillBordersOnWorkers();
	void WaitForAllWorkersToFinish();
	void ExpandLight(ExpandedLight* out, WorldMeshLight* worldMeshLight);
	void BuildEffectedChunksFieldForLight(BitField* out, ExpandedLight* light);
	void WriteOutputFile(int lightAtlasIndex);

	Config config;
	const char* assetsFolderPath;
	ILogger* logger;
	IWorldMeshAsset* worldMeshAsset;
	IRayTracer* rayTracer;
	IThreadManager* threadManager;
	ITimestampProvider* timestampProvider;
	//DynamicLengthArray<ILightAtlas*> lightAtlases;
	ILightAtlas* currentLightAtlas;
	//DynamicLengthArray<Light*> lights;
	FixedLengthArray<IWorker*, 16> workers;
};