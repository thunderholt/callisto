#pragma once

#include "Include/Callisto.h"

class NullPhysicalGamePad : public IPhysicalGamePad
{
public:
	NullPhysicalGamePad();
	virtual void Init();
	virtual bool GetIsActive();
	virtual void Poll();
	virtual Vec2* GetLeftStickAxes();
	virtual Vec2* GetRightStickAxes();
	virtual float GetTriggerPressure(GamePadTrigger trigger);

private:
	Vec2 leftStickAxes;
	Vec2 rightStickAxes;
};