#ifdef ANDROID
#include "Input/NullMouse.h"

NullMouse::NullMouse()
{
	Vec2::Zero(&this->movementOffset);
}

void NullMouse::Init()
{
	// Noop.
}

bool NullMouse::GetIsActive()
{
	return false;
}

void NullMouse::Poll()
{
	// Noop.
}

Vec2* NullMouse::GetMovementOffset()
{
	return &this->movementOffset;
}

MouseButtonState NullMouse::GetMouseButtonState(MouseButton button)
{
	return MouseButtonStateUp;
}

#endif