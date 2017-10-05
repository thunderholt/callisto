#ifdef WIN32

#include "Input/WindowsKeyboard.h"

WindowsKeyboard::WindowsKeyboard()
{
	for (int i = 0; i < 128; i++)
	{
		//this->keyDownTimestamps[i] = 0.0;
		this->keyStates[i] = KeyStateUp;
	}
}

WindowsKeyboard::~WindowsKeyboard()
{
	// Noop.
}

void WindowsKeyboard::Init()
{
	// Noop.
}

bool WindowsKeyboard::GetIsActive()
{
	return true;
}

KeyState WindowsKeyboard::GetKeyState(char key)
{
	//return this->keyDownTimestamps[key] > 0.0 ? KeyStateDown : KeyStateUp;
	return this->keyStates[key];
}

/*double WindowsKeyboard::GetKeyDownTimestamp(char key)
{
	return this->keyDownTimestamps[key];
}*/

int WindowsKeyboard::GetNumberOfEvents()
{
	return this->keyEvents.GetLength();
}

KeyEvent* WindowsKeyboard::GetEvent(int index)
{
	return &this->keyEvents[index];
}

void WindowsKeyboard::ClearEvents()
{
	this->keyEvents.Clear();
}

void WindowsKeyboard::RaiseKeyDownEvent(char key)
{
	//IEngine* engine = GetEngine();
	//ITimestampProvider* timestampProvider = engine->GetTimestampProvider();

	//this->keyDownTimestamps[key] = timestampProvider->GetTimestampMillis();
	this->keyStates[key] = KeyStateDown;

	KeyEvent* keyEvent = &this->keyEvents.PushAndGet();
	keyEvent->key = key;
	keyEvent->keyState = KeyStateDown;
}

void WindowsKeyboard::RaiseKeyUpEvent(char key)
{
	//this->keyDownTimestamps[key] = 0.0;
	this->keyStates[key] = KeyStateUp;

	KeyEvent* keyEvent = &this->keyEvents.PushAndGet();
	keyEvent->key = key;
	keyEvent->keyState = KeyStateUp;
}

#endif