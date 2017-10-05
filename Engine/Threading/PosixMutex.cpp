#ifdef ANDROID

#include "Threading/PosixMutex.h"

PosixMutex::PosixMutex()
{
	pthread_mutex_init(&this->mutex, null);
}

PosixMutex::~PosixMutex()
{

}

void PosixMutex::WaitForOwnership()
{
	pthread_mutex_lock(&this->mutex);
}

void PosixMutex::Release()
{
	pthread_mutex_unlock(&this->mutex);
}

#endif