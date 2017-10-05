#pragma once

#include "Include/PVSBuilder.h"

enum WorkerTaskType
{
	WorkerTaskTypeNone,
	WorkerTaskTypeComputeSectorInsidePoints,
	WorkerTaskTypeRunSectorCruncher
};

class Worker : public IWorker
{
public:
	Worker();
	virtual ~Worker();
	virtual void Init(int startSectorIndex, int numberOfSectorsToCrunch);
	virtual void ComputeSectorInsidePointsAsync();
	virtual void RunSectorCruncherAsync(SectorCruncherType sectorCruncherType);
	virtual bool GetHasFinished();
	virtual void RunThreadEntryPoint();

private:
	void ComputeSectorInsidePointsInternal();
	void RunSectorCruncherInternal();
	void CreateRandomPointWithinSector(Vec3* out, SectorMetrics* sectorMetrics, Sector* sector);

	WorkerTaskType currentTaskType;
	bool currentTaskHasFinished;
	ISectorCruncher* sectorCruncher;
	int startSectorIndex;
	int numberOfSectorsToCrunch;
};
