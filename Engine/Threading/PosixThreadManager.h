#pragma once
#ifdef ANDROID

#include "Include/Callisto.h"

class PosixThreadManager : public IThreadManager
{
public:
	virtual ~PosixThreadManager();
	virtual void StartThread(ThreadEntryPointFunction entryPoint, void* arg);
	virtual void Sleep(unsigned long duration);
	virtual bool GetThreadingIsSupported();
};

#endif