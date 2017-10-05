#ifdef EMSCRIPTEN

#include <emscripten.h>
#include <html5.h>
#include "Input/WebBrowserMouse.h"

Vec2 globalMovementOffset;

EM_BOOL OnMouseEvent(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
	if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE)
	{
		globalMovementOffset.x = (float)mouseEvent->movementX;
		globalMovementOffset.y = (float)mouseEvent->movementY;
		Vec2::Scale(&globalMovementOffset, &globalMovementOffset, 6.5f);
	}

	return true;
}

WebBrowserMouse::WebBrowserMouse()
{
	Vec2::Zero(&this->movementOffset);
}

void WebBrowserMouse::Init()
{
	emscripten_set_mousemove_callback(null, null, false, OnMouseEvent);
}

bool WebBrowserMouse::GetIsActive()
{
	return true;
}

void WebBrowserMouse::Poll()
{
	this->movementOffset = globalMovementOffset;
	Vec2::Zero(&globalMovementOffset);
}

Vec2* WebBrowserMouse::GetMovementOffset()
{
	return &this->movementOffset;
}

MouseButtonState WebBrowserMouse::GetMouseButtonState(MouseButton button)
{
	return MouseButtonStateUp; // FIXME
}

#endif