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
	virtual void Init(int startMeshChunkIndex, int numberOfMeshChunkIndexes);
	virtual void ComputeLightIslandsAsync();
	virtual bool GetHasFinished();
	virtual void RunThreadEntryPoint();

private:
	void ComputeLightIslandsInternal();
	
	WorkerTaskType currentTaskType;
	bool currentTaskHasFinished;
	int startMeshChunkIndex;
	int numberOfMeshChunkIndexes;
};
