#pragma once
#ifdef WIN32

#include "Include/Callisto.h"

class WindowsThreadManager : public IThreadManager
{
public:
	virtual ~WindowsThreadManager();
	virtual void StartThread(ThreadEntryPointFunction entryPoint, void* arg);
	virtual void Sleep(unsigned long duration);
	virtual bool GetThreadingIsSupported();
};

#endif