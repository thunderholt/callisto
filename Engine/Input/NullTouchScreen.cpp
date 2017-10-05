#ifdef WIN32

#include "Input/NullTouchScreen.h"

NullTouchScreen::~NullTouchScreen()
{
}

void NullTouchScreen::Init()
{
	// Noop.
}

bool NullTouchScreen::GetIsActive()
{
	return false;
}

int NullTouchScreen::GetNumberOfEvents()
{
	return 0;
}

TouchEvent* NullTouchScreen::GetEvent(int index)
{
	return null;
}

void NullTouchScreen::ClearEvents()
{
	// Noop.
}

#endif