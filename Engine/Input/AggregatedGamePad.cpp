#include "Input/AggregatedGamePad.h"

AggregatedGamePad::AggregatedGamePad()
{
	//Vec3::Zero(&this->movementMultipliers.translation);
	//Vec3::Zero(&this->movementMultipliers.rotation);
	//this->mouseRotationSensitivity = 0.0015f;

	Vec2::Zero(&this->leftStickAxes);
	Vec2::Zero(&this->rightStickAxes);
}

/*void AggregatedGamePad::AggregatedGamePad()
{
	//Vec3::Zero(&this->movementMultipliers.translation);
	//Vec3::Zero(&this->movementMultipliers.rotation);

	//this->EvaluateKeyboardInput();
	//this->EvaluateMouseInput();
}*/

void AggregatedGamePad::Init()
{
	// Noop.
}

bool AggregatedGamePad::GetIsActive()
{
	return true;
}

void AggregatedGamePad::Poll()
{
	IEngine* engine = GetEngine();

	Vec2::Zero(&this->leftStickAxes);
	Vec2::Zero(&this->rightStickAxes);

	this->triggerPressures[GamePadTriggerLeft] = 0.0f;
	this->triggerPressures[GamePadTriggerRight] = 0.0f;

	this->EvaluateGamePad(engine->GetTouchScreenGamePad());
	this->EvaluateGamePad(engine->GetPhysicalGamePad());
	this->EvaluateGamePad(engine->GetKeyboardMouseGamePad());
}

Vec2* AggregatedGamePad::GetLeftStickAxes()
{
	return &this->leftStickAxes;
}

Vec2* AggregatedGamePad::GetRightStickAxes()
{
	return &this->rightStickAxes;
}

float AggregatedGamePad::GetTriggerPressure(GamePadTrigger trigger)
{
	return this->triggerPressures[trigger];
}

void AggregatedGamePad::EvaluateGamePad(IGamePad* gamePad)
{
	if (gamePad->GetIsActive())
	{
		Vec2* leftStickAxes = gamePad->GetLeftStickAxes();
		Vec2* rightStickAxes = gamePad->GetRightStickAxes();

		if (!Vec2::IsZero(leftStickAxes))
		{
			this->leftStickAxes = *leftStickAxes;
		}

		if (!Vec2::IsZero(rightStickAxes))
		{
			this->rightStickAxes = *rightStickAxes;
		}

		float leftTriggerPressure = gamePad->GetTriggerPressure(GamePadTriggerLeft);
		float rightTriggerPressure = gamePad->GetTriggerPressure(GamePadTriggerRight);

		if (leftTriggerPressure != 0.0f)
		{
			this->triggerPressures[GamePadTriggerLeft] = leftTriggerPressure;
		}

		if (rightTriggerPressure != 0.0f)
		{
			this->triggerPressures[GamePadTriggerRight] = rightTriggerPressure;
		}
	}
}

/*InputMovementMultipliers* AggregatedGamePad::GetMovementMultipliers()
{
	return &this->movementMultipliers;
}*/

/*void AggregatedGamePad::EvaluateKeyboardInput()
{
	IEngine* engine = GetEngine();
	IKeyboard* keyboard = engine->GetKeyboard();

	char activeLeftRightKey = this->GetLatestKeydown('A', 'D');
	char activeforwardBackwardKey = this->GetLatestKeydown('W', 'S');

	if (activeLeftRightKey == 'A')
	{
		this->movementMultipliers.translation.x = -1.0f;
	}
	else if (activeLeftRightKey == 'D')
	{
		this->movementMultipliers.translation.x = 1.0f;
	}

	if (activeforwardBackwardKey == 'W')
	{
		this->movementMultipliers.translation.z = 1.0f;
	}
	else if (activeforwardBackwardKey == 'S')
	{
		this->movementMultipliers.translation.z = -1.0f;
	}
	*/
	/*for (int keyEventIndex = 0; keyEventIndex < keyboard->GetNumberOfEvents(); keyEventIndex++)
	{
		KeyEvent* keyEvent = keyboard->GetEvent(keyEventIndex);
		if (keyEvent->keyState == KeyStateDown)
		{
			if (keyEvent->key == 'A')
			{
				this->movementMultipliers.translation.x = -1.0f;
			}
			else if (keyEvent->key == 'D')
			{
				this->movementMultipliers.translation.x = 1.0f;
			}
			else if (keyEvent->key == 'W')
			{
				this->movementMultipliers.translation.z = 1.0f;
			}
			else if (keyEvent->key == 'S')
			{
				this->movementMultipliers.translation.z = -1.0f;
			}
		}
		else if (keyEvent->keyState == KeyStateUp)
		{
			if (keyEvent->key == 'A')
			{
				this->movementMultipliers.translation.x = keyboard->GetKeyState('D') == KeyStateDown ? 1.0f : 0.0f;
			}
			else if (keyEvent->key == 'D')
			{
				this->movementMultipliers.translation.x = keyboard->GetKeyState('A') == KeyStateDown ? -1.0f : 0.0f;
			}
			else if (keyEvent->key == 'W')
			{
				this->movementMultipliers.translation.z = keyboard->GetKeyState('S') == KeyStateDown ? -1.0f : 0.0f;
			}
			else if (keyEvent->key == 'S')
			{
				this->movementMultipliers.translation.z = keyboard->GetKeyState('W') == KeyStateDown ? 1.0f : 0.0f;
			}
		}
	}*/
//}

/*void InputAggregator::EvaluateMouseInput()
{
	IEngine* engine = GetEngine();
	IMouse* mouse = engine->GetMouse();

	Vec2 movementOffset = *mouse->GetMovementOffset();
	Vec2::Scale(&movementOffset, &movementOffset, mouseRotationSensitivity);

	this->movementMultipliers.rotation.y = movementOffset.x;
	this->movementMultipliers.rotation.x = movementOffset.y;
}

char InputAggregator::GetLatestKeydown(char key1, char key2)
{
	IEngine* engine = GetEngine();
	IKeyboard* keyboard = engine->GetKeyboard();

	char latestKey = -1;

	double keyDownTimestamp1 = keyboard->GetKeyDownTimestamp(key1);
	double keyDownTimestamp2 = keyboard->GetKeyDownTimestamp(key2);

	if (keyDownTimestamp1 > keyDownTimestamp2)
	{
		latestKey = key1;
	}
	else if (keyDownTimestamp2 > keyDownTimestamp1)
	{
		latestKey = key2;
	}

	return latestKey;
}*/