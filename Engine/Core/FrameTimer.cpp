#include "Core/FrameTimer.h"

FrameTimer::FrameTimer()
{
	this->lastFrameTimestamp = 0;
	this->numberOfFrames = 0;
	this->numberOfFpsHitches = 0;
	this->summedFps = 0;
	this->currentFps = 0;
	this->minFps = -1;
	this->maxFps = -1;
	this->averageFps = 0;
	this->lastFrameDurationMillis = 0;
}

FrameTimer::~FrameTimer()
{
	// Noop.
}

void FrameTimer::StartFrame()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();
	ITimestampProvider* timestampProvider = engine->GetTimestampProvider();

	double currentFrameTimestamp = timestampProvider->GetTimestampMillis();

	if (this->lastFrameTimestamp != 0.0)
	{
		// Calculate the current FPS.
		this->lastFrameDurationMillis = (float)(currentFrameTimestamp - this->lastFrameTimestamp);
		
		// it seems as though the Adnroid emulator can return some wierd timestamps sometimes.
		// Hopefully this only happens on the emulator.
		if (this->lastFrameDurationMillis < 0.0f)
		{
			this->lastFrameDurationMillis = 0.0001f; // To avoid division by zero issues.
		}

		this->currentFps = 1000.0f / this->lastFrameDurationMillis;

		// Update the min FPS.
		if (this->minFps == -1.0f || this->currentFps < this->minFps)
		{
			this->minFps = this->currentFps;
		}

		// Update the max FPS.
		if (this->maxFps == -1.0f || this->currentFps > this->maxFps)
		{
			this->maxFps = this->currentFps;
		}

		// Update the average FPS.
		this->numberOfFrames++;
		this->summedFps += this->currentFps;
		this->averageFps = (float)(this->summedFps / (double)this->numberOfFrames);

		// Update the number of FPS hitches.
		if (this->currentFps < 50.0f)
		{
			this->numberOfFpsHitches++;
		}
	}

	this->lastFrameTimestamp = currentFrameTimestamp;
}


int FrameTimer::GetNumberOfFrames()
{
	return this->numberOfFrames;
}

int FrameTimer::GetNumberOfFpsHitches()
{
	return this->numberOfFpsHitches;
}

float FrameTimer::GetFpsHitchRate()
{
	return (float)this->numberOfFpsHitches / this->numberOfFrames * 100.0f;
}

float FrameTimer::GetCurrentFPS()
{
	return this->currentFps;
}

float FrameTimer::GetMinFPS()
{
	return this->minFps;
}

float FrameTimer::GetMaxFPS()
{
	return this->maxFps;
}

float FrameTimer::GetAverageFPS()
{
	return this->averageFps;
}

float FrameTimer::GetLastFrameDurationMillis()
{
	return this->lastFrameDurationMillis;
}

float FrameTimer::GetMultiplier()
{
	return this->lastFrameDurationMillis / 16.6667f;
}