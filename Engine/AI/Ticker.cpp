#include <stdlib.h> 
#include "AI/Ticker.h"

Ticker::Ticker(float ticksPerSecond)
{
	this->ticksPerSecond = ticksPerSecond;
	this->t = -1.0f;
}

Ticker::~Ticker()
{
}

bool Ticker::Tick()
{
	IEngine* engine = GetEngine();
	IFrameTimer* frameTimer = engine->GetFrameTimer();

	bool isTick = false;

	if (this->t == -1.0f) 
	{
		float random = rand() / (float)RAND_MAX;
		this->t = this->CalculateT() * random;
	}
	else 
	{
		this->t -= frameTimer->GetLastFrameDurationMillis();
		if (this->t <= 0.0f) 
		{
			this->t += this->CalculateT();
			isTick = true;
		}
	}

	return isTick;
}

void Ticker::Reset()
{
	this->t = this->CalculateT();
}

float Ticker::CalculateT()
{
	return 1000.0f / this->ticksPerSecond;
}