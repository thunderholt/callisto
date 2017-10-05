#ifdef WIN32
#include <windows.h>
#include "Input/WindowsMouse.h"

WindowsMouse::WindowsMouse()
{
	Vec2::Zero(&this->movementOffset);
}

/*void Mouse::RaiseMouseMoveEvent(Vec2* movementOffset)
{
	this->movementOffset = *movementOffset;
}*/

void WindowsMouse::Init()
{
	// Noop.
}

bool WindowsMouse::GetIsActive()
{
	return true;
}

void WindowsMouse::Poll()
{
	float halfScreenWidth = (float)GetSystemMetrics(SM_CXSCREEN) / 2.0f;
	float halfScreenHeight = (float)GetSystemMetrics(SM_CYSCREEN) / 2.0f;

	POINT cursorPos;
	GetCursorPos(&cursorPos);

	this->movementOffset.x = (float)cursorPos.x - halfScreenWidth;
	this->movementOffset.y = (float)cursorPos.y - halfScreenHeight;

	SetCursorPos((int)halfScreenWidth, (int)halfScreenHeight);

	this->buttonStates[MouseButtonLeft] = GetAsyncKeyState(VK_LBUTTON) != 0 ? MouseButtonStateDown : MouseButtonStateUp;
	this->buttonStates[MouseButtonRight] = GetAsyncKeyState(VK_RBUTTON) != 0 ? MouseButtonStateDown : MouseButtonStateUp;
}

Vec2* WindowsMouse::GetMovementOffset()
{
	return &this->movementOffset;
}

MouseButtonState WindowsMouse::GetMouseButtonState(MouseButton button)
{
	return this->buttonStates[button];
}

/*void Mouse::ClearMovementOffset()
{
	Vec2::Zero(&this->movementOffset);
}*/

/*
void Mouse::RaiseMouseMoveEvent(Vec2 movementOffset)
{
	MouseEvent* mouseEvent = &this->mouseEvents.Push();
	mouseEvent->eventType = MouseEventTypeMove;
	mouseEvent->movementOffset = movementOffset;
}

int Mouse::GetNumberOfEvents()
{
	return this->mouseEvents.GetLength();
}

MouseEvent* Mouse::GetEvent(int index)
{
	return &this->mouseEvents[index];
}

void Mouse::ClearEvents()
{
	this->mouseEvents.Clear();
}
*/

#endif