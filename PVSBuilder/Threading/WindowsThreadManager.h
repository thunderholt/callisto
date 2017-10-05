#pragma once
#ifdef WIN32

#include "Include/PVSBuilder.h"

class WindowsThreadManager : public IThreadManager
{
public:
	virtual ~WindowsThreadManager();
	virtual void StartThread(ThreadEntryPointFunction entryPoint, void* arg);
	virtual void Sleep(unsigned long duration);
};

#endif