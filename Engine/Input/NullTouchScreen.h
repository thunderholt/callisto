#pragma once
#ifdef WIN32

#include "Include/Callisto.h"

class NullTouchScreen : public ITouchScreen
{
public:
	virtual ~NullTouchScreen();
	virtual void Init();
	virtual bool GetIsActive();
	virtual int GetNumberOfEvents();
	virtual TouchEvent* GetEvent(int index);
	virtual void ClearEvents();
};

#endif

