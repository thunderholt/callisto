#pragma once
#ifdef WIN32

#include "Include/PVSBuilder.h"

class WindowsLogger : public ILogger
{
public:
	virtual void Write(const char *format, ...);
};

#endif