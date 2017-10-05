#include "Input/TouchScreenGamePad.h"

TouchScreenGamePad::TouchScreenGamePad()
{
	/*this->leftStickTouchId = -1;
	this->rightStickTouchId = -1;
	this->fireStickTouchId = -1;

	this->stickRadius = 30.0f;
	this->stickRadiusSqr = this->stickRadius * this->stickRadius;
	Vec2::Zero(&leftStickOrigin);
	Vec2::Zero(&rightStickOrigin);
	Vec2::Zero(&fireStickOrigin);
	Vec2::Zero(&rightStickAnchorPoint);
	Vec2::Zero(&fireStickAnchorPoint);*/

	Vec2::Zero(&this->stickAxeses[TouchScreenGamePadLeftStickIndex]);
	Vec2::Zero(&this->stickAxeses[TouchScreenGamePadRightStickIndex]);

	this->triggerPressures[GamePadTriggerLeft] = 0;
	this->triggerPressures[GamePadTriggerRight] = 0;

	memset(this->sticks, 0, sizeof(TouchScreenGamePadStick) * 2);
}

void TouchScreenGamePad::Init()
{
	IEngine* engine = GetEngine();
	IRasterizer* rasterizer = engine->GetRasterizer();
	Vec2* viewportSize = rasterizer->GetViewportSize();

	float deadZoneHalfWidth = 5.0f;

	// Init the left stick.
	TouchScreenGamePadStick* leftStick = this->GetLeftStick();
	Vec2::Set(&leftStick->activationZone.from, 0, 0);
	Vec2::Set(&leftStick->activationZone.to, viewportSize->x / 2.0f - deadZoneHalfWidth, viewportSize->y);
	leftStick->movementMode = TouchScreenGamePadStickMovementModeElastic;
	leftStick->movementElasticRadius = 150.0f;

	// Init the right stick.
	TouchScreenGamePadStick* rightStick = this->GetRightStick();
	Vec2::Set(&rightStick->activationZone.from, viewportSize->x / 2.0f + deadZoneHalfWidth, 0);
	Vec2::Set(&rightStick->activationZone.to, viewportSize->x, viewportSize->y);
	rightStick->movementMode = TouchScreenGamePadStickMovementModeDrag;
	rightStick->movementDragMultiplier = 0.1f;
	rightStick->hasButton = true;
	rightStick->buttonTrigger = GamePadTriggerRight;
	Vec2::Set(&rightStick->buttonPosition, viewportSize->x - 100.0f, viewportSize->y - 200.0f);
	rightStick->buttonRadius = 100;

	/*Vec2::Set(&leftStickOrigin, 100.0f, viewportSize->y - 100.0f);
	Vec2::Set(&rightStickOrigin, viewportSize->x - 100.0f, viewportSize->y - 100.0f);
	Vec2::Set(&fireStickOrigin, viewportSize->x - 100.0f, viewportSize->y - 200.0f);*/
}

bool TouchScreenGamePad::GetIsActive()
{
	IEngine* engine = GetEngine();
	ITouchScreen* touchScreen = engine->GetTouchScreen();

	return touchScreen->GetIsActive();
}

void TouchScreenGamePad::Poll()
{
	IEngine* engine = GetEngine();
	ITouchScreen* touchScreen = engine->GetTouchScreen();
	ILogger* logger = engine->GetLogger();

	if (touchScreen->GetIsActive())
	{
		for (int stickIndex = 0; stickIndex < 2; stickIndex++)
		{
			TouchScreenGamePadStick* stick = &this->sticks[stickIndex];
			Vec2* stickAxes = &this->stickAxeses[stickIndex];

			if (stick->movementMode == TouchScreenGamePadStickMovementModeDrag)
			{
				Vec2::Zero(stickAxes);
			}

			for (int touchEventIndex = 0; touchEventIndex < touchScreen->GetNumberOfEvents(); touchEventIndex++)
			{
				TouchEvent* touchEvent = touchScreen->GetEvent(touchEventIndex);

				if (touchEvent->type == TouchEventTypeStart)
				{
					if (Rect::PointIsWithin(&stick->activationZone, &touchEvent->position) && !stick->isActive)
					{
						logger->Write("Started stick: %d", stickIndex);

						stick->isActive = true;
						stick->activeTouchId = touchEvent->touchId;
						stick->movementAnchorPoint = touchEvent->position;

						if (stick->hasButton)
						{
							float touchPointDistanceToButton = 
								Vec2::Distance(&stick->buttonPosition, &touchEvent->position);

							if (touchPointDistanceToButton <= stick->buttonRadius)
							{
								stick->buttonIsPressed = true;
								this->triggerPressures[stick->buttonTrigger] = 1.0f;
							}
						}
					}
				}
				else if (touchEvent->type == TouchEventTypeMove)
				{
					if (stick->isActive && stick->activeTouchId == touchEvent->touchId)
					{
						if (stick->movementMode == TouchScreenGamePadStickMovementModeElastic)
						{
							this->CalculateStickAxesFromTouchOffset(
								stickAxes, &stick->movementAnchorPoint,
								&touchEvent->position, stick->movementElasticRadius, true, 1.0f);
						}
						else if (stick->movementMode == TouchScreenGamePadStickMovementModeDrag)
						{
							this->CalculateStickAxesFromTouchOffset(
								stickAxes, &stick->movementAnchorPoint,
								&touchEvent->position, 0, false, stick->movementDragMultiplier);

							stick->movementAnchorPoint = touchEvent->position;
						}
					}
				}
				else if (touchEvent->type == TouchEventTypeEnd)
				{
					if (stick->isActive && stick->activeTouchId == touchEvent->touchId)
					{
						logger->Write("Ended stick: %d", stickIndex);

						Vec2::Zero(stickAxes);
						stick->isActive = false;

						if (stick->hasButton)
						{
							stick->buttonIsPressed = false;
							this->triggerPressures[stick->buttonTrigger] = 0.0f;
						}
					}
				}
			}
		}

		/*Vec2::Zero(&this->rightStickAxes);

		for (int touchEventIndex = 0; touchEventIndex < touchScreen->GetNumberOfEvents(); touchEventIndex++)
		{
			TouchEvent* touchEvent = touchScreen->GetEvent(touchEventIndex);
			if (touchEvent->type == TouchEventTypeStart)
			{
				if (Vec2::DistanceSqr(&this->leftStickOrigin, &touchEvent->position) <= this->stickRadiusSqr)
				{
					this->leftStickTouchId = touchEvent->touchId;
				}
				else if (Vec2::DistanceSqr(&this->rightStickOrigin, &touchEvent->position) <= this->stickRadiusSqr)
				{
					this->rightStickTouchId = touchEvent->touchId;
					rightStickAnchorPoint = touchEvent->position;
				}
				else if (Vec2::DistanceSqr(&this->fireStickOrigin, &touchEvent->position) <= this->stickRadiusSqr)
				{
					this->fireStickTouchId = touchEvent->touchId;
					fireStickAnchorPoint = touchEvent->position;
					this->triggerPressures[GamePadTriggerRight] = 1.0f;
				}
			}
			else if (touchEvent->type == TouchEventTypeEnd)
			{
				if (this->leftStickTouchId == touchEvent->touchId)
				{
					this->leftStickTouchId = -1;
					Vec2::Zero(&this->leftStickAxes);
				}
				else if (this->rightStickTouchId == touchEvent->touchId)
				{
					this->rightStickTouchId = -1;
					Vec2::Zero(&this->rightStickAxes);
				}
				else if (this->fireStickTouchId == touchEvent->touchId)
				{
					this->fireStickTouchId = -1;
					Vec2::Zero(&this->rightStickAxes);
					this->triggerPressures[GamePadTriggerRight] = 0.0f;
				}
			}

			if (this->leftStickTouchId == touchEvent->touchId)
			{
				this->CalculateStickAxesFromTouchPosition(
					&this->leftStickAxes, &this->leftStickOrigin, &touchEvent->position, true);
			}
			else if (this->rightStickTouchId == touchEvent->touchId)
			{
				this->CalculateStickAxesFromTouchPosition(
					&this->rightStickAxes, &this->rightStickAnchorPoint, &touchEvent->position, false);

				this->rightStickAnchorPoint = touchEvent->position;
			}
			else if (this->fireStickTouchId == touchEvent->touchId)
			{
				this->CalculateStickAxesFromTouchPosition(
					&this->rightStickAxes, &this->fireStickAnchorPoint, &touchEvent->position, false);

				this->fireStickAnchorPoint = touchEvent->position;
			}
		}*/
	}
}

Vec2* TouchScreenGamePad::GetLeftStickAxes()
{
	return &this->stickAxeses[TouchScreenGamePadLeftStickIndex];
}

Vec2* TouchScreenGamePad::GetRightStickAxes()
{
	return &this->stickAxeses[TouchScreenGamePadRightStickIndex];
}
/*
Vec2* TouchScreenGamePad::GetLeftStickOrigin()
{
	return &this->leftStickOrigin;
}

Vec2* TouchScreenGamePad::GetRightStickOrigin()
{
	return &this->rightStickOrigin;
}

Vec2* TouchScreenGamePad::GetFireStickOrigin()
{
	return &this->fireStickOrigin;
}*/

float TouchScreenGamePad::GetTriggerPressure(GamePadTrigger trigger)
{
	return this->triggerPressures[trigger];
}

TouchScreenGamePadStick* TouchScreenGamePad::GetLeftStick()
{
	return &this->sticks[TouchScreenGamePadLeftStickIndex];
}

TouchScreenGamePadStick* TouchScreenGamePad::GetRightStick()
{
	return &this->sticks[TouchScreenGamePadRightStickIndex];
}

void TouchScreenGamePad::CalculateStickAxesFromTouchOffset(
	Vec2* out, Vec2* stickAnchorPoint, Vec2* touchPosition, float maxOffsetDistance, bool invertY, float multiplier)
{
	Vec2 offset;
	Vec2::Sub(&offset, touchPosition, stickAnchorPoint);

	if (maxOffsetDistance > 0)
	{
		float offsetLength = Math::Min(Vec2::Length(&offset) / maxOffsetDistance, 1.0f);

		Vec2::Normalize(&offset, &offset);
		Vec2::Scale(&offset, &offset, offsetLength);
	}

	out->x = offset.x;
	out->y = offset.y;

	if (invertY)
	{
		out->y *= -1.0f;
	}

	Vec2::Scale(out, out, multiplier);
}

/*
void TouchScreenGamePad::CalculateStickAxesFromTouchPosition(Vec2* out, Vec2* stickOrigin, Vec2* touchPosition, bool invertY)
{
	Vec2 offset;
	Vec2::Sub(&offset, touchPosition, stickOrigin);
	
	float offsetLength = Math::Min(Vec2::Length(&offset) / this->stickRadius, 1.0f);


	Vec2::Normalize(&offset, &offset);
	Vec2::Scale(&offset, &offset, offsetLength);

	out->x = offset.x;
	out->y = offset.y;

	if (invertY)
	{
		out->y *= -1.0f;
	}
}*/