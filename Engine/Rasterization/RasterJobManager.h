#pragma once

#include "Include/Callisto.h"

class RasterJobManager : public IRasterJobManager
{
public:
	RasterJobManager();
	virtual ~RasterJobManager();

	virtual RasterJob* GetCurrentFrameRasterJob();
	virtual RasterJob* GetNextFrameRasterJob();
	virtual void SwapJobs();

private:
	RasterJob rasterJobs[2];
	int currentFrameJobIndex;
	int nextFrameJobIndex;
};