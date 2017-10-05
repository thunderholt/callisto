#include "Rasterization/RasterJobManager.h"

RasterJobManager::RasterJobManager()
{
	this->currentFrameJobIndex = 0;
	this->nextFrameJobIndex = 1;
}

RasterJobManager::~RasterJobManager()
{
	// Noop.
}

RasterJob* RasterJobManager::GetCurrentFrameRasterJob()
{
	return &this->rasterJobs[this->currentFrameJobIndex];
}

RasterJob* RasterJobManager::GetNextFrameRasterJob()
{
	return &this->rasterJobs[this->nextFrameJobIndex];
}

void RasterJobManager::SwapJobs()
{
	int temp = this->nextFrameJobIndex;
	this->nextFrameJobIndex = this->currentFrameJobIndex;
	this->currentFrameJobIndex = temp;
}