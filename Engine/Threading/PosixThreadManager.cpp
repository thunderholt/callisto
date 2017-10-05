#ifdef ANDROID

#include <pthread.h>
#include <unistd.h>
#include "Threading/PosixThreadManager.h"

struct ThreadEntryPointWrapperData
{
    void (*entryPoint)(void* arg);
    void* arg;
};

ThreadEntryPointWrapperData threadEntryPointWrapperData;

void *ThreadEntryPointWrapper(void* arg)
{
    threadEntryPointWrapperData.entryPoint(threadEntryPointWrapperData.arg);

    return 0;
}

PosixThreadManager::~PosixThreadManager()
{
	// Noop.
}

void PosixThreadManager::StartThread(ThreadEntryPointFunction entryPoint, void* arg)
{
    pthread_t thread;

    threadEntryPointWrapperData.entryPoint = entryPoint;
    threadEntryPointWrapperData.arg = arg;

    pthread_create(&thread, null, ThreadEntryPointWrapper, null);
}

void PosixThreadManager::Sleep(unsigned long duration)
{
	usleep(duration * 1000);
}

bool PosixThreadManager::GetThreadingIsSupported()
{
	return true;
}

#endif