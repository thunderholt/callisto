#ifdef EMSCRIPTEN

#include <emscripten.h>
#include <html5.h>
#include "Input/WebBrowserPhysicalGamePad.h"

WebBrowserPhysicalGamePad::WebBrowserPhysicalGamePad()
{
	this->isActive = false;
	Vec2::Zero(&this->leftStickAxes);
	Vec2::Zero(&this->rightStickAxes);
}

void WebBrowserPhysicalGamePad::Init()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	int numberOfGamePads = emscripten_get_num_gamepads();
	logger->Write("Number of physical gamepads: %d.", numberOfGamePads);

	if (numberOfGamePads > 0)
	{
		this->isActive = true;
	}
}
	
bool WebBrowserPhysicalGamePad::GetIsActive()
{
	return this->isActive;
}

void WebBrowserPhysicalGamePad::Poll()
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	Vec2::Zero(&this->leftStickAxes);
	Vec2::Zero(&this->rightStickAxes);

	if (this->isActive)
	{
		EmscriptenGamepadEvent gamePadState;

		if (emscripten_get_gamepad_status(0, &gamePadState) == EMSCRIPTEN_RESULT_SUCCESS)
		{
			/*if (gamePadState.axis[0] != 0)
			{
				logger->Write("Axes 0: %f", gamePadState.axis[0]);
			}*/

			this->leftStickAxes.x = this->ApplyDeadzoneToAxis(gamePadState.axis[0]);
			this->leftStickAxes.y = this->ApplyDeadzoneToAxis(-gamePadState.axis[1]);

			this->rightStickAxes.x = this->ApplyDeadzoneToAxis(gamePadState.axis[2]);
			this->rightStickAxes.y = this->ApplyDeadzoneToAxis(gamePadState.axis[3]);
		}
	}
}

Vec2* WebBrowserPhysicalGamePad::GetLeftStickAxes()
{
	return &this->leftStickAxes;
}

Vec2* WebBrowserPhysicalGamePad::GetRightStickAxes()
{
	return &this->rightStickAxes;
}

float WebBrowserPhysicalGamePad::GetTriggerPressure(GamePadTrigger trigger)
{
	return 0.0f; // FIXME
}

double WebBrowserPhysicalGamePad::ApplyDeadzoneToAxis(double axis)
{
	return (axis > 0.2 || axis < -0.2) ? axis : 0.0;
}

#endif