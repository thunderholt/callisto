#pragma once
#ifdef WIN32

#include <windows.h>
#include "Include/Callisto.h"

class WindowsMutex : public IMutex
{
public:
	WindowsMutex();
	virtual ~WindowsMutex();
	virtual void WaitForOwnership();
	virtual void Release();

private:
	HANDLE mutex;
};

#endif