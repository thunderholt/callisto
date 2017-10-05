#pragma once
#ifdef WIN32

#include <windows.h>
#include "Include/Callisto.h"

class WindowsSemaphore : public ISemaphore
{
public:
	WindowsSemaphore();
	virtual ~WindowsSemaphore();
	virtual void Wait();
	virtual void Signal();

private:
	HANDLE event;
};

#endif