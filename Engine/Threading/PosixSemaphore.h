#pragma once
#ifdef ANDROID

#include <semaphore.h>
#include "Include/Callisto.h"

class PosixSemaphore : public ISemaphore
{
public:
	PosixSemaphore();
	virtual ~PosixSemaphore();
	virtual void Wait();
	virtual void Signal();

private:
	sem_t semaphore;
};

#endif