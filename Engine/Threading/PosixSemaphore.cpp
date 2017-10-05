#ifdef ANDROID

#include "Threading/PosixSemaphore.h"

PosixSemaphore::PosixSemaphore()
{
	sem_init(&this->semaphore, 0, 0);
}

PosixSemaphore::~PosixSemaphore()
{

}

void PosixSemaphore::Wait()
{
	sem_wait(&this->semaphore);
}

void PosixSemaphore::Signal()
{
	sem_post(&this->semaphore);
}

#endif