#pragma once
#ifdef EMSCRIPTEN

#include "Include/Callisto.h"

class WebBrowserTouchScreen : public ITouchScreen
{
public:
	virtual void Init();
	virtual bool GetIsActive();
	virtual int GetNumberOfEvents();
	virtual TouchEvent* GetEvent(int index);
	virtual void ClearEvents();
	void RaiseTouchEvent(TouchEventType type, int touchId, int x, int y);

private:
	DynamicLengthArray<TouchEvent> touchEvents;
};

#endif

