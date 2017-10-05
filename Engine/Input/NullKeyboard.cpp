#ifdef ANDROID
#include "Input/NullKeyboard.h"

NullKeyboard::NullKeyboard()
{
	// Noop.
}

NullKeyboard::~NullKeyboard()
{
	// Noop.
}

void NullKeyboard::Init()
{
	// Noop.
}

bool NullKeyboard::GetIsActive()
{
	return false;
}

KeyState NullKeyboard::GetKeyState(char key)
{
	return KeyStateUp;
}

/*double NullKeyboard::GetKeyDownTimestamp(char key)
{
	return 0.0f;
}*/

int NullKeyboard::GetNumberOfEvents()
{
	return 0;
}

KeyEvent* NullKeyboard::GetEvent(int index)
{
	return null;
}

void NullKeyboard::ClearEvents()
{
	// Noop.
}

#endif