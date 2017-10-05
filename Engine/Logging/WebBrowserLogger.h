#pragma once
#ifdef EMSCRIPTEN

#include "Include/Callisto.h"

class WebBrowserLogger : public ILogger
{
public:
	virtual void Write(const char *format, ...);
};

#endif