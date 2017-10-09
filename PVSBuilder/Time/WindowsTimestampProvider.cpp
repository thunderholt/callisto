#ifdef WIN32

#include <windows.h>
#include "Time/WindowsTimestampProvider.h"

double timerFrequency = -1;

double WindowsTimestampProvider::GetTimestampMillis()
{
	if (timerFrequency == -1)
	{
		LARGE_INTEGER fli;
		QueryPerformanceFrequency(&fli);

		timerFrequency = (double)fli.QuadPart / 1000;
	}

	LARGE_INTEGER cli;
	QueryPerformanceCounter(&cli);
	double timestamp = double(cli.QuadPart) / timerFrequency;

	return timestamp;
}

#endif