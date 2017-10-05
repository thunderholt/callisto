#ifdef EMSCRIPTEN
#include <stdio.h>
#include <stdarg.h>
#include "Logging/WebBrowserLogger.h" 

void WebBrowserLogger::Write(const char *format, ...)
{
	char buffer[1024];

	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 1024, format, args);
	va_end(args);

	printf("%s", buffer);
	printf("\n");
}

#endif