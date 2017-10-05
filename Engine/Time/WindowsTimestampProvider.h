#pragma once
#ifdef WIN32

#include "Include/Callisto.h"

class WindowsTimestampProvider : public ITimestampProvider
{
public:
	virtual double GetTimestampMillis();
};

#endif