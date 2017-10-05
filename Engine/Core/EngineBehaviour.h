#pragma once

#include "Include/Callisto.h"

class EngineBehaviour : public IEngineBehaviour, IRunnable
{
public:
	EngineBehaviour();
	virtual ~EngineBehaviour();

	virtual bool PostCoreAssetLoadInit();
	//virtual void CleanUp();
	virtual void StartProducingNextFrame(RasterJob* rasterJob);
	virtual void WaitForProductionOfNextFrameToFinish();
	virtual void Run();

private:
	//bool isInitialised;
	IWorker* worker;
	RasterJob* rasterJob;
};