#ifdef WIN32

#include <windows.h> 
#include <process.h> 
#include "Threading/WindowsThreadManager.h"

WindowsThreadManager::~WindowsThreadManager()
{
	// Noop
}

void WindowsThreadManager::StartThread(ThreadEntryPointFunction entryPoint, void* arg)
{
	_beginthread(entryPoint, 0, arg);
}

void WindowsThreadManager::Sleep(unsigned long duration)
{
	::Sleep(duration);
}

bool WindowsThreadManager::GetThreadingIsSupported()
{
	return true;
}

#endif