#pragma once
#ifdef EMSCRIPTEN

#include "Include/Callisto.h"

class WebBrowserThreadManager : public IThreadManager
{
public:
	virtual ~WebBrowserThreadManager();
	virtual void StartThread(ThreadEntryPointFunction entryPoint, void* arg);
	virtual void Sleep(unsigned long duration);
	virtual bool GetThreadingIsSupported();
};

#endif