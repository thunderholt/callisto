#pragma once
#ifdef ANDROID

#include "Include/Callisto.h"

class PosixTimestampProvider : public ITimestampProvider
{
public:
	virtual double GetTimestampMillis();
};

#endif