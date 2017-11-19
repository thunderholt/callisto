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
	virtual ILightAtlas* GetLightAtlas(int index);
	virtual IThreadManager* GetThreadManager();
	virtual ITimestampProvider* GetTimestampProvider();
	virtual const char* GetAssetsFolderPath();

private:
	void InitWorkers();
	void InitLightAtlases();
	void ComputeLightIslandsOnWorkers();
	void WaitForAllWorkersToFinish();
	void WriteOutputFiles();

	const char* assetsFolderPath;
	ILogger* logger;
	IWorldMeshAsset* worldMeshAsset;
	IThreadManager* threadManager;
	ITimestampProvider* timestampProvider;
	DynamicLengthArray<ILightAtlas*> lightAtlases;
	FixedLengthArray<IWorker*, 16> workers;
};