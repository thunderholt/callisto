#pragma once

#include "Include/PVSBuilder.h"

class Engine : public IEngine
{
public:
	Engine();
	virtual ~Engine();
	virtual void BuildPVS(const char* worldMeshAssetFilePath, const char* assetsFolderPath, const char* outputFilePath);
	virtual ILogger* GetLogger();
	virtual IWorldMeshAsset* GetWorldMeshAsset();
	virtual ISectorVisibilityLookup* GetSectorVisibilityLookup();
	//virtual SectorMetrics* GetSectorMetrics();
	virtual IThreadManager* GetThreadManager();
	virtual ITimestampProvider* GetTimestampProvider();
	virtual Sector* GetSector(int index);
	virtual int GetNumberOfSectors();
	virtual const char* GetAssetsFolderPath();
	//virtual Vec3* GetPointCompletelyOutsideOfCollisionMeshExtremities();

private:
	//void InitSectorMetrics();
	void InitWorkers();
	void InitSectors();
	void ComputeSectorInsidePointsOnWorkers();
	void RunSectorCruncherOnWorkers(SectorCruncherType sectorCruncherType);
	void WaitForAllWorkersToFinish();
	//void ComputeSectorOutputVariables();
	//void BuildSectorResidentWorldMeshChunkIndexes();
	void BuildSectorVisibleSectors();
	void WriteOutputFile();

	const char* outputFilePath;
	const char* assetsFolderPath;
	ILogger* logger;
	IWorldMeshAsset* worldMeshAsset;
	ISectorVisibilityLookup* sectorVisibilityLookup;
	IThreadManager* threadManager;
	ITimestampProvider* timestampProvider;
	//SectorMetrics sectorMetrics;
	DynamicLengthArray<Sector*> sectors;
	//Vec3 pointCompletelyOutsideOfCollisionMeshExtremities;
	FixedLengthArray<IWorker*, 16> workers;
	//int numberOfVisibleSectorIndexes;
};