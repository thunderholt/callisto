#pragma once
#ifdef ANDROID

#include "Include/Callisto.h"

class NullMouse : public IMouse
{
public:
	NullMouse();
	virtual void Init();
	virtual bool GetIsActive();
	virtual void Poll();
	virtual Vec2* GetMovementOffset();
	virtual MouseButtonState GetMouseButtonState(MouseButton button);

private:
	Vec2 movementOffset;
};

#endif