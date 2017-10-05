#ifdef EMSCRIPTEN
#include "Threading/WebBrowserThreadManager.h"

WebBrowserThreadManager::~WebBrowserThreadManager()
{
	// Noop.
}

void WebBrowserThreadManager::StartThread(ThreadEntryPointFunction entryPoint, void* arg)
{
    // Noop.
}

void WebBrowserThreadManager::Sleep(unsigned long duration)
{
	// Noop.
}

bool WebBrowserThreadManager::GetThreadingIsSupported()
{
	return false;
}

#endif