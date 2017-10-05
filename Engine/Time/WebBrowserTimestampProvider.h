#pragma once
#ifdef EMSCRIPTEN

#include "Include/Callisto.h"

class WebBrowserTimestampProvider : public ITimestampProvider
{
public:
	virtual double GetTimestampMillis();
};

#endif