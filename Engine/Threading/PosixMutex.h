#pragma once
#ifdef ANDROID

#include <pthread.h>
#include "Include/Callisto.h"

class PosixMutex : public IMutex
{
public:
	PosixMutex();
	virtual ~PosixMutex();
	virtual void WaitForOwnership();
	virtual void Release();

private:
	pthread_mutex_t mutex;
};

#endif