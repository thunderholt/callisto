#include <stdio.h>
#include <emscripten.h>
//#include <trace.h>
#include <html5.h>
#include <GL/glfw.h>
#include "Include/Callisto.h"

/*EM_BOOL OnKeyDown(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
	IEngine* engine = GetEngine();
	IKeyboard* keyboard = engine->GetKeyboard();
	keyboard->RaiseKeyDownEvent(keyEvent->code[3]);
	return true;
}

EM_BOOL OnKeyUp(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
	IEngine* engine = GetEngine();
	IKeyboard* keyboard = engine->GetKeyboard();
	keyboard->RaiseKeyUpEvent(keyEvent->code[3]);
	return true;
}*/

/*EM_BOOL OnMouseEvent(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
	IEngine* engine = GetEngine();
	IMouse* mouse = engine->GetMouse();
	ILogger* logger = engine->GetLogger();

	if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE)
	{
		Vec2 movementOffset;
		movementOffset.x = (float)mouseEvent->movementX;
		movementOffset.y = (float)mouseEvent->movementY;
		Vec2::Scale(&movementOffset, &movementOffset, 3.5f);
		mouse->RaiseMouseMoveEvent(&movementOffset);

		//logger->Write("%f,%f", movementOffset.x, movementOffset.y);
	}

	return true;
}*/

/*EM_BOOL OnPointerLockChanged(int eventType, const EmscriptenPointerlockChangeEvent* pointerlockChangeEvent, void* userData)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	if (pointerlockChangeEvent->isActive)
	{
		logger->Write("OnPointerLockChanged:active");
	}
	else
	{
		logger->Write("OnPointerLockChanged:inactive");
	}

	return true;
}*/

/*EM_BOOL OnPointerLockError(int eventType, const void* reserved, void* userData)
{
	IEngine* engine = GetEngine();
	ILogger* logger = engine->GetLogger();

	logger->Write("OnPointerLockError");

	return true;
}*/

void HeartBeat()
{
	//emscripten_trace_report_memory_layout();
	//emscripten_trace_report_off_heap_data();

	IEngine* engine = GetEngine();
	if (!engine->GetMustTerminate())
	{
		engine->Heartbeat();

		glfwSwapBuffers();
	}
	else
	{
		printf("Engine must terminate, cancelling main loop.\n");
		emscripten_cancel_main_loop();

		DestroyEngine();
		printf("Destroyed engine.\n");

		DestroyFactory();
		printf("Destroyed factory.\n");

		glfwTerminate();
	}

}

int main() 
{
	//emscripten_trace_configure_for_google_wtf();

	if (!glfwInit()) 
	{
	    printf("Could not initialize GLFW.\n");
	    return -1;
	}

	/* Create a windowed mode window and its OpenGL context */
	int screenWidth, screenHeight, isFullscreen;
	emscripten_get_canvas_size(&screenWidth, &screenHeight, &isFullscreen);

	printf("Screen size: %d x %d\n", screenWidth, screenHeight);

	if(glfwOpenWindow(screenWidth, screenHeight, 0, 0, 0, 0, 16, 0, GLFW_WINDOW) != GL_TRUE)
	{
	    printf("Could not create OpenGL window\n");
	    glfwTerminate();
	    return -1;
	}

	CreateFactory();
	printf("Created factory.\n");

	CreateEngine();
	printf("Created engine.\n");

	InitGame();
	printf("Game initialised.\n");

	IEngine* engine = GetEngine();
	engine->Init();

	//emscripten_set_pointerlockchange_callback("canvas", null, false, OnPointerLockChanged);
	//emscripten_set_pointerlockerror_callback("canvas", null, false, OnPointerLockError);
	//emscripten_request_pointerlock("canvas", true);
	//emscripten_request_fullscreen(null, true);

	//emscripten_set_keydown_callback(null, null, false, OnKeyDown);
	//emscripten_set_keyup_callback(null, null, false, OnKeyUp);
	//emscripten_set_mousemove_callback(null, null, false, OnMouseEvent);
	emscripten_set_main_loop(HeartBeat, 0, 1);

	return 0;
}