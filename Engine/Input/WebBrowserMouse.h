#pragma once
#ifdef EMSCRIPTEN

#include "Include/Callisto.h"

class WebBrowserMouse : public IMouse
{
public:
	WebBrowserMouse();
	virtual void Init();
	virtual bool GetIsActive();
	virtual void Poll();
	virtual Vec2* GetMovementOffset();
	virtual MouseButtonState GetMouseButtonState(MouseButton button);

private:
	Vec2 movementOffset;
};

#endif