#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <html5.h>
#include "Input/WebBrowserKeyboard.h"

EM_BOOL OnKeyDown(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
	/*double* keyDownTimestamps = (double*)userData;

	IEngine* engine = GetEngine();
	ITimestampProvider* timestampProvider = engine->GetTimestampProvider();

	char key = keyEvent->code[3];
	keyDownTimestamps[key] = timestampProvider->GetTimestampMillis();*/

	char key = keyEvent->code[3];

	WebBrowserKeyboard* keyboard = (WebBrowserKeyboard*)userData;
	keyboard->RaiseKeyDownEvent(key);

	return true;
}

EM_BOOL OnKeyUp(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
	/*double* keyDownTimestamps = (double*)userData;

	char key = keyEvent->code[3];
	keyDownTimestamps[key] = 0;*/

	char key = keyEvent->code[3];

	WebBrowserKeyboard* keyboard = (WebBrowserKeyboard*)userData;
	keyboard->RaiseKeyUpEvent(key);

	return true;
}

WebBrowserKeyboard::WebBrowserKeyboard()
{
	for (int i = 0; i < 128; i++)
	{
		//this->keyDownTimestamps[i] = 0.0;
		this->keyStates[i] = KeyStateUp;
	}
}

WebBrowserKeyboard::~WebBrowserKeyboard()
{
	// Noop.
}

void WebBrowserKeyboard::Init()
{
	emscripten_set_keydown_callback(null, this, false, OnKeyDown);
	emscripten_set_keyup_callback(null, this, false, OnKeyUp);
}

bool WebBrowserKeyboard::GetIsActive()
{
	return true;
}

KeyState WebBrowserKeyboard::GetKeyState(char key)
{
	//return this->keyDownTimestamps[key] > 0.0 ? KeyStateDown : KeyStateUp;
	return this->keyStates[key];
}

/*double WebBrowserKeyboard::GetKeyDownTimestamp(char key)
{
	return this->keyDownTimestamps[key];
}*/

int WebBrowserKeyboard::GetNumberOfEvents()
{
	return this->keyEvents.GetLength();
}

KeyEvent* WebBrowserKeyboard::GetEvent(int index)
{
	return &this->keyEvents[index];
}

void WebBrowserKeyboard::ClearEvents()
{
	this->keyEvents.Clear();
}

void WebBrowserKeyboard::RaiseKeyDownEvent(char key)
{
	//IEngine* engine = GetEngine();
	//ITimestampProvider* timestampProvider = engine->GetTimestampProvider();

	//this->keyDownTimestamps[key] = timestampProvider->GetTimestampMillis();
	this->keyStates[key] = KeyStateDown;

	KeyEvent* keyEvent = &this->keyEvents.PushAndGet();
	keyEvent->key = key;
	keyEvent->keyState = KeyStateDown;
}

void WebBrowserKeyboard::RaiseKeyUpEvent(char key)
{
	//this->keyDownTimestamps[key] = 0.0;
	this->keyStates[key] = KeyStateUp;

	KeyEvent* keyEvent = &this->keyEvents.PushAndGet();
	keyEvent->key = key;
	keyEvent->keyState = KeyStateUp;
}

#endif