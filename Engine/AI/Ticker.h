#pragma once

#include "Include/Callisto.h"

class Ticker : public ITicker
{
public:
	Ticker(float ticksPerSecond);
	virtual ~Ticker();
	virtual bool Tick();
	virtual void Reset();

private:
	float CalculateT();

	float ticksPerSecond;
	float t;
};