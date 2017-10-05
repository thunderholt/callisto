#include "Input/NullPhysicalGamePad.h"

NullPhysicalGamePad::NullPhysicalGamePad()
{
	Vec2::Zero(&this->leftStickAxes);
	Vec2::Zero(&this->rightStickAxes);
}

void NullPhysicalGamePad::Init()
{
	// Noop.
}

bool NullPhysicalGamePad::GetIsActive()
{
	return false;
}

void NullPhysicalGamePad::Poll()
{

}

Vec2* NullPhysicalGamePad::GetLeftStickAxes()
{
	return &this->leftStickAxes;
}

Vec2* NullPhysicalGamePad::GetRightStickAxes()
{
	return &this->rightStickAxes;
}

float NullPhysicalGamePad::GetTriggerPressure(GamePadTrigger trigger)
{
	return 0.0f;
}