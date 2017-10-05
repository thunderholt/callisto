#pragma once

#include "Include/Callisto.h"

#define TouchScreenGamePadLeftStickIndex 0
#define TouchScreenGamePadRightStickIndex 1

class TouchScreenGamePad : public ITouchScreenGamePad
{
public:
	TouchScreenGamePad();
	virtual void Init();
	virtual bool GetIsActive();
	virtual void Poll();
	virtual Vec2* GetLeftStickAxes();
	virtual Vec2* GetRightStickAxes();
	/*virtual Vec2* GetLeftStickOrigin();
	virtual Vec2* GetRightStickOrigin();
	virtual Vec2* GetFireStickOrigin();*/
	virtual float GetTriggerPressure(GamePadTrigger trigger);

	virtual TouchScreenGamePadStick* GetLeftStick();
	virtual TouchScreenGamePadStick* GetRightStick();

private:
	void CalculateStickAxesFromTouchOffset(Vec2* out, Vec2* stickAnchorPoint, Vec2* touchPosition, float maxOffsetDistance, bool invertY, float multiplier);
	/*

	float stickRadius;
	float stickRadiusSqr;

	int leftStickTouchId;
	int rightStickTouchId;
	int fireStickTouchId;

	Vec2 leftStickOrigin;
	Vec2 rightStickOrigin;
	Vec2 fireStickOrigin;

	Vec2 rightStickAnchorPoint;
	Vec2 fireStickAnchorPoint;*/

	TouchScreenGamePadStick sticks[2];
	Vec2 stickAxeses[2];
	float triggerPressures[2];
};
