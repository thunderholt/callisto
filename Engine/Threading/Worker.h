#pragma once

#include "Include/Callisto.h"

class Worker : public IWorker
{
public:
	Worker(IRunnable* runnable);
	virtual ~Worker();
	virtual void Start();
	virtual void Stop();
	virtual void DoWork();
	virtual void WaitForWorkToComplete();
	void Run();

private:
	IRunnable* runnable;
	bool isStarted;
	volatile bool shouldTerminate;
	volatile bool hasTerminated;
	volatile bool workIsComplete;
	ISemaphore* semaphore;
};