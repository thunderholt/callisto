#pragma once
#ifdef WIN32

#include "Include/LightAtlasBuilder.h"

class WindowsTimestampProvider : public ITimestampProvider
{
public:
	virtual double GetTimestampMillis();
};

#endif