#ifdef WIN32

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include "Logging/WindowsLogger.h" 

void WindowsLogger::Write(const char *format, ...)
{
	char buffer[1024];

	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 1024, format, args);
	va_end(args);

	//OutputDebugStringA(buffer);
	//OutputDebugStringA("\n");
	printf(buffer);
	printf("\n");
}

#endif