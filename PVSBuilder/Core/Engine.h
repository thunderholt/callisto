#pragma once

#include "Include/PVSBuilder.h"

class Engine : public IEngine
{
public:
	Engine();
	virtual ~Engine();
	virtual void BuildPVS(const char* worldMeshAssetFilePath, SectorMetrics sectorMetrics);
	virtual ILogger* GetLogger();
	virtual IWorldMeshAsset* GetWorldMeshAsset();
	virtual ISectorVisibilityLookup* GetSectorVisibilityLookup();
	virtual SectorMetrics* GetSectorMetrics();
	virtual IThreadManager* GetThreadManager();
	virtual ITimestampProvider* GetTimestampProvider();
	virtual Sector* GetSectors();
	//virtual Vec3* GetPointCompletelyOutsideOfCollisionMeshExtremities();

private:
	void InitWorkers();
	void InitSectors();
	void ComputeSectorInsidePointsOnWorkers();
	void RunSectorCruncherOnWorkers(SectorCruncherType sectorCruncherType);
	void WaitForAllWorkersToFinish();
	void ComputeSectorOutputVariables();
	void WriteOutputFile();

	ILogger* logger;
	IWorldMeshAsset* worldMeshAsset;
	ISectorVisibilityLookup* sectorVisibilityLookup;
	IThreadManager* threadManager;
	ITimestampProvider* timestampProvider;
	SectorMetrics sectorMetrics;
	Sector* sectors;
	//Vec3 pointCompletelyOutsideOfCollisionMeshExtremities;
	FixedLengthArray<IWorker*, 16> workers;
	//int numberOfVisibleSectorIndexes;
};