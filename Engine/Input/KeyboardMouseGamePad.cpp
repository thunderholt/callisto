#include "Input/KeyboardMouseGamePad.h"

KeyboardMouseGamePad::KeyboardMouseGamePad()
{
	Vec2::Zero(&this->leftStickAxes);
	Vec2::Zero(&this->rightStickAxes);
	this->mouseRotationSensitivity = 0.02f;
}

void KeyboardMouseGamePad::Init()
{
	// Noop.
}

bool KeyboardMouseGamePad::GetIsActive()
{
	IEngine* engine = GetEngine();
	IKeyboard* keyboard = engine->GetKeyboard();
	IMouse* mouse = engine->GetMouse();

	return keyboard->GetIsActive() && mouse->GetIsActive();
}

void KeyboardMouseGamePad::Poll()
{
	if (this->GetIsActive())
	{
		this->EvaluateKeyboardInput();
		this->EvaluateMouseInput();
	}
}

Vec2* KeyboardMouseGamePad::GetLeftStickAxes()
{
	return &this->leftStickAxes;
}

Vec2* KeyboardMouseGamePad::GetRightStickAxes()
{
	return &this->rightStickAxes;
}

float KeyboardMouseGamePad::GetTriggerPressure(GamePadTrigger trigger)
{
	return this->triggerPressures[trigger];
}

void KeyboardMouseGamePad::EvaluateKeyboardInput()
{
	IEngine* engine = GetEngine();
	IKeyboard* keyboard = engine->GetKeyboard();

	for (int keyEventIndex = 0; keyEventIndex < keyboard->GetNumberOfEvents(); keyEventIndex++)
	{
		KeyEvent* keyEvent = keyboard->GetEvent(keyEventIndex);
		if (keyEvent->keyState == KeyStateDown)
		{
			if (keyEvent->key == 'A')
			{
				this->leftStickAxes.x = -1.0f;
			}
			else if (keyEvent->key == 'D')
			{
				this->leftStickAxes.x = 1.0f;
			}
			else if (keyEvent->key == 'W')
			{
				this->leftStickAxes.y = 1.0f;
			}
			else if (keyEvent->key == 'S')
			{
				this->leftStickAxes.y = -1.0f;
			}
		}
		else if (keyEvent->keyState == KeyStateUp)
		{
			if (keyEvent->key == 'A')
			{
				this->leftStickAxes.x = keyboard->GetKeyState('D') == KeyStateDown ? 1.0f : 0.0f;
			}
			else if (keyEvent->key == 'D')
			{
				this->leftStickAxes.x = keyboard->GetKeyState('A') == KeyStateDown ? -1.0f : 0.0f;
			}
			else if (keyEvent->key == 'W')
			{
				this->leftStickAxes.y = keyboard->GetKeyState('S') == KeyStateDown ? -1.0f : 0.0f;
			}
			else if (keyEvent->key == 'S')
			{
				this->leftStickAxes.y = keyboard->GetKeyState('W') == KeyStateDown ? 1.0f : 0.0f;
			}
		}
	}
}

void KeyboardMouseGamePad::EvaluateMouseInput()
{
	IEngine* engine = GetEngine();
	IMouse* mouse = engine->GetMouse();

	this->rightStickAxes = *mouse->GetMovementOffset();
	Vec2::Scale(&this->rightStickAxes, &this->rightStickAxes, this->mouseRotationSensitivity);

	this->triggerPressures[GamePadTriggerRight] =
		mouse->GetMouseButtonState(MouseButtonLeft) == MouseButtonStateDown ? 1.0f : 0.0f;

	this->triggerPressures[GamePadTriggerLeft] =
		mouse->GetMouseButtonState(MouseButtonRight) == MouseButtonStateDown ? 1.0f : 0.0f;
}