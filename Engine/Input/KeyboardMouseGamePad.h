#pragma once

#include "Include/Callisto.h"

class KeyboardMouseGamePad : public IKeyboardMouseGamePad
{
public:
	KeyboardMouseGamePad();
	virtual void Init();
	virtual bool GetIsActive();
	virtual void Poll();
	virtual Vec2* GetLeftStickAxes();
	virtual Vec2* GetRightStickAxes();
	virtual float GetTriggerPressure(GamePadTrigger trigger);

private:
	void EvaluateKeyboardInput();
	void EvaluateMouseInput();

	Vec2 leftStickAxes;
	Vec2 rightStickAxes;
	float mouseRotationSensitivity;
	float triggerPressures[2];
};