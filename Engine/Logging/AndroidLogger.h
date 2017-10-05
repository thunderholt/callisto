#pragma once
#ifdef ANDROID

#include "Include/Callisto.h"

class AndroidLogger : public ILogger
{
public:
	virtual void Write(const char *format, ...);
};

#endif