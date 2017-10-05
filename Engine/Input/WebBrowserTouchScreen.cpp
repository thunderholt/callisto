#ifdef EMSCRIPTEN

#include <emscripten.h>
#include <html5.h>
#include "Input/WebBrowserTouchScreen.h"

EM_BOOL OnTouchStart(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData)
{
	//IEngine* engine = GetEngine();
	//ILogger* logger = engine->GetLogger();
	WebBrowserTouchScreen* touchScreen = (WebBrowserTouchScreen*)userData;

	for (int i = 0; i < touchEvent->numTouches; i++)
	{
		const EmscriptenTouchPoint* touchPoint = &touchEvent->touches[i];
		if (touchPoint->isChanged)
		{
			touchScreen->RaiseTouchEvent(TouchEventTypeStart, touchPoint->identifier, touchPoint->clientX, touchPoint->clientY);
			//logger->Write("OnTouchStart: %d", touchPoint->identifier);
		}
	}

	return true;
}

EM_BOOL OnTouchEnd(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData)
{
	//IEngine* engine = GetEngine();
	//ILogger* logger = engine->GetLogger();
	WebBrowserTouchScreen* touchScreen = (WebBrowserTouchScreen*)userData;

	for (int i = 0; i < touchEvent->numTouches; i++)
	{
		const EmscriptenTouchPoint* touchPoint = &touchEvent->touches[i];
		if (touchPoint->isChanged)
		{
			touchScreen->RaiseTouchEvent(TouchEventTypeEnd, touchPoint->identifier, touchPoint->clientX, touchPoint->clientY);
			//logger->Write("OnTouchEnd: %d", touchPoint->identifier);
		}
	}

	return true;
}

EM_BOOL OnTouchMove(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData)
{
	//IEngine* engine = GetEngine();
	//ILogger* logger = engine->GetLogger();
	WebBrowserTouchScreen* touchScreen = (WebBrowserTouchScreen*)userData;

	for (int i = 0; i < touchEvent->numTouches; i++)
	{
		const EmscriptenTouchPoint* touchPoint = &touchEvent->touches[i];
		if (touchPoint->isChanged)
		{
			touchScreen->RaiseTouchEvent(TouchEventTypeMove, touchPoint->identifier, touchPoint->clientX, touchPoint->clientY);
			//logger->Write("OnTouchMove: %d", touchPoint->identifier);
		}
	}

	return true;
}

void WebBrowserTouchScreen::Init()
{
	emscripten_set_touchstart_callback("canvas", this, false, OnTouchStart);
	emscripten_set_touchend_callback("canvas", this, false, OnTouchEnd);
	emscripten_set_touchmove_callback("canvas", this, false, OnTouchMove);
}

bool WebBrowserTouchScreen::GetIsActive()
{
	return true;
}

int WebBrowserTouchScreen::GetNumberOfEvents()
{
	return this->touchEvents.GetLength();
}

TouchEvent* WebBrowserTouchScreen::GetEvent(int index)
{
	return &this->touchEvents[index];
}

void WebBrowserTouchScreen::ClearEvents()
{
	this->touchEvents.Clear();
}

void WebBrowserTouchScreen::RaiseTouchEvent(TouchEventType type, int touchId, int x, int y)
{
	TouchEvent* touchEvent = &this->touchEvents.PushAndGet();
	touchEvent->type = type;
	touchEvent->touchId = touchId;
	touchEvent->position.x = (float)x;
	touchEvent->position.y = (float)y;
}

#endif