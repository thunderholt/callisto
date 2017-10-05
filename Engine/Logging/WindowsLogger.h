#pragma once
#ifdef WIN32

#include "Include/Callisto.h"

class WindowsLogger : public ILogger
{
public:
	virtual void Write(const char *format, ...);
};

#endif