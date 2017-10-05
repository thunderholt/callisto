#ifdef ANDROID

#include "Input/AndroidTouchScreen.h"

void AndroidTouchScreen::Init()
{
	// Noop.
}

bool AndroidTouchScreen::GetIsActive()
{
	return true;
}

int AndroidTouchScreen::GetNumberOfEvents()
{
	return this->touchEvents.GetLength();
}

TouchEvent* AndroidTouchScreen::GetEvent(int index)
{
	return &this->touchEvents[index];
}

void AndroidTouchScreen::ClearEvents()
{
	this->touchEvents.Clear();
}

void AndroidTouchScreen::RaiseTouchEvent(TouchEventType type, int touchId, int x, int y)
{
	TouchEvent* touchEvent = &this->touchEvents.PushAndGet();
	touchEvent->type = type;
	touchEvent->touchId = touchId;
	touchEvent->position.x = (float)x;
	touchEvent->position.y = (float)y;
}

#endif