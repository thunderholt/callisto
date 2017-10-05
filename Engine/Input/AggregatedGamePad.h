#pragma once

#include "Include/Callisto.h"

class AggregatedGamePad : public IAggregatedGamePad
{
public:
	AggregatedGamePad();
	virtual void Init();
	virtual bool GetIsActive();
	virtual void Poll();
	//virtual InputMovementMultipliers* GetMovementMultipliers();
	virtual Vec2* GetLeftStickAxes();
	virtual Vec2* GetRightStickAxes();
	virtual float GetTriggerPressure(GamePadTrigger trigger);
	//virtual ButtonState GetButtonState(int buttonIndex);

private:
	void EvaluateGamePad(IGamePad* gamePad);
	//void EvaluateKeyboardInput();
	//void EvaluateMouseInput();
	//char GetLatestKeydown(char key1, char key2);

	//InputMovementMultipliers movementMultipliers;
	//float mouseRotationSensitivity;
	Vec2 leftStickAxes;
	Vec2 rightStickAxes;
	float triggerPressures[2];
};