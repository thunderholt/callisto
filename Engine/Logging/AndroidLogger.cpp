#ifdef ANDROID

#include <stdio.h>
#include <stdarg.h>
#include <android/log.h>
#include "Logging/AndroidLogger.h"

void AndroidLogger::Write(const char *format, ...)
{
	char buffer[1024];

	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 1024, format, args);
	va_end(args);

	__android_log_print(ANDROID_LOG_DEBUG, "Callisto", "%s", buffer);
}

#endif