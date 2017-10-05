#ifdef WIN32

#include "Threading/WindowsSemaphore.h"

WindowsSemaphore::WindowsSemaphore()
{
	this->event = CreateEvent(0, false, false, 0);
}

WindowsSemaphore::~WindowsSemaphore()
{
	CloseHandle(this->event);
}

void WindowsSemaphore::Wait()
{
	WaitForSingleObject(this->event, INFINITE);
}

void WindowsSemaphore::Signal()
{
	SetEvent(this->event);
}

#endif