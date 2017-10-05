#pragma once

#include "Include/Callisto.h"

class FrameTimer : public IFrameTimer
{
public:
	FrameTimer();
	virtual ~FrameTimer();
	virtual void StartFrame();
	virtual int GetNumberOfFrames();
	virtual int GetNumberOfFpsHitches();
	virtual float GetFpsHitchRate();
	virtual float GetCurrentFPS();
	virtual float GetMinFPS();
	virtual float GetMaxFPS();
	virtual float GetAverageFPS();
	virtual float GetLastFrameDurationMillis();
	virtual float GetMultiplier();

private:
	double lastFrameTimestamp;
	int numberOfFrames;
	int numberOfFpsHitches;
	double summedFps;
	float currentFps;
	float minFps;
	float maxFps;
	float averageFps;
	float lastFrameDurationMillis;
};