#pragma once
#ifdef EMSCRIPTEN

#include "Include/Callisto.h"

class WebBrowserPhysicalGamePad : public IPhysicalGamePad
{
public:
	WebBrowserPhysicalGamePad();
	virtual void Init();
	virtual bool GetIsActive();
	virtual void Poll();
	virtual Vec2* GetLeftStickAxes();
	virtual Vec2* GetRightStickAxes();
	virtual float GetTriggerPressure(GamePadTrigger trigger);

private:
	double ApplyDeadzoneToAxis(double axis);

	bool isActive;
	Vec2 leftStickAxes;
	Vec2 rightStickAxes;
};

#endif