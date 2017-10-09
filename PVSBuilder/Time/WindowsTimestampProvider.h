#pragma once
#ifdef WIN32

#include "Include/PVSBuilder.h"

class WindowsTimestampProvider : public ITimestampProvider
{
public:
	virtual double GetTimestampMillis();
};

#endif