#ifdef ANDROID

#include <math.h>
#include <time.h>
#include "Time/PosixTimestampProvider.h"

double PosixTimestampProvider::GetTimestampMillis()
{
	struct timespec spec;

	clock_gettime(CLOCK_MONOTONIC, &spec);

	double timestamp = spec.tv_nsec / 1.0e6; // Convert nanoseconds to milliseconds

	return timestamp;
}

#endif