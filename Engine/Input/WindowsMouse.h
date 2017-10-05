#pragma once
#ifdef WIN32

#include "Include/Callisto.h"

class WindowsMouse : public IMouse
{
public:
	WindowsMouse();
	//virtual void RaiseMouseMoveEvent(Vec2* movementOffset);
	virtual void Init();
	virtual bool GetIsActive();
	virtual void Poll();
	virtual Vec2* GetMovementOffset();
	virtual MouseButtonState GetMouseButtonState(MouseButton button);
	//virtual void ClearMovementOffset();
	
	//virtual int GetNumberOfEvents();
	//virtual MouseEvent* GetEvent(int index);
	//virtual void ClearEvents();

private:
	Vec2 movementOffset;
	MouseButtonState buttonStates[2];
	//Array<MouseEvent> mouseEvents;
};

#endif