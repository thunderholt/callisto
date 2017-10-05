#ifdef EMSCRIPTEN

#include "Time/WebBrowserTimestampProvider.h"

extern "C"
{
	 void jsGetTimestampMillis(double* out);
}

double WebBrowserTimestampProvider::GetTimestampMillis()
{
	double d = 0;
	jsGetTimestampMillis(&d);
	return d;
}

#endif