#ifdef WIN32

#include "Threading/WindowsMutex.h"

WindowsMutex::WindowsMutex()
{
	this->mutex = CreateMutex(null, false, null);
}

WindowsMutex::~WindowsMutex()
{
	CloseHandle(this->mutex);
}

void WindowsMutex::WaitForOwnership()
{
	WaitForSingleObject(this->mutex, INFINITE);
}

void WindowsMutex::Release()
{
	ReleaseMutex(this->mutex);
}

#endif
