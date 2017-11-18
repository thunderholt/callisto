/*
* GL01Hello.cpp: Test OpenGL C/C++ Setup
*/
#define WIN32_LEAN_AND_MEAN  
#include <windows.h>  // For MS Windows
#include <tchar.h>
#include <crtdbg.h> 

#include <stdio.h>
#include <GL/glew.h>
#include "Include/Callisto.h"
#include "Input/WindowsKeyboard.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HINSTANCE hInstance, HWND* hWnd, HDC* hDC, HGLRC* hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
//void UpdateMouse();

typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int iCmdShow)
{
	HDC hDC;
	HGLRC hRC;
	HWND hWnd;
	MSG msg;
	BOOL quit = FALSE;

	msg.wParam = 0;

	// enable OpenGL for the window
	EnableOpenGL(hInstance, &hWnd, &hDC, &hRC);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// Problem: glewInit failed, something is seriously wrong.
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 0;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	ShowCursor(false);

	CreateFactory();
	CreateEngine();
	InitGame();

	IEngine* engine = GetEngine();

	EngineStartUpArgs startUpArgs;
	memset(&startUpArgs, 0, sizeof(EngineStartUpArgs));

	if (__argc == 2)
	{ 
		StringUtil::CopyString(startUpArgs.startUpMapPath, __argv[1], AssetMaxFilePathLength);
	}

	engine->Init(&startUpArgs);

	// program main loop
	while (!quit)
	{
		// check for messages
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// handle or dispatch messages
			if (msg.message == WM_QUIT)
			{
				quit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		//UpdateMouse();

		// OpenGL animation code goes here
		engine->Heartbeat();

		SwapBuffers(hDC);

		_ASSERTE(_CrtCheckMemory());

		if (engine->GetMustTerminate())
		{
			quit = true;
		}


	}
	

	//engine->CleanUp();
	DestroyEngine();
	DestroyFactory();

	// shutdown OpenGL
	DisableOpenGL(hWnd, hDC, hRC);

	// destroy the window explicitly
	DestroyWindow(hWnd);

	OutputDebugStringA("Dumping memory leaks...");
	_CrtDumpMemoryLeaks();
	OutputDebugStringA("... done.");

	return msg.wParam;
}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	IEngine* engine = GetEngine();
	

	switch (message)
	{

	case WM_CREATE:
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
		if (engine != null)
		{
			WindowsKeyboard* keyboard = (WindowsKeyboard*)engine->GetKeyboard();
			if (keyboard != null)
			{
				keyboard->RaiseKeyDownEvent((char)wParam);
			}
		}
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;
	case WM_KEYUP:
		if (engine != null)
		{
			WindowsKeyboard* keyboard = (WindowsKeyboard*)engine->GetKeyboard();
			if (keyboard != null)
			{
				keyboard->RaiseKeyUpEvent((char)wParam);
			}
		}
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	}

}

// Enable OpenGL

void EnableOpenGL(HINSTANCE hInstance, HWND* hWnd, HDC* hDC, HGLRC* hRC)
{
	// register window class
	WNDCLASS wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T("Callisto");
	RegisterClass(&wc);

	// create main window
	*hWnd = CreateWindow(
		_T("Callisto"), _T("Callisto"),
		WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
		//0, 0, 1152, 648,
		0, 0, 1900, 1000,
		NULL, NULL, hInstance, NULL);

	PIXELFORMATDESCRIPTOR pfd;
	int format;

	// get the device context (DC)
	*hDC = GetDC(*hWnd);

	// set the pixel format for the DC
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat(*hDC, &pfd);
	SetPixelFormat(*hDC, format, &pfd);

	// create and enable the render context (RC)
	*hRC = wglCreateContext(*hDC);
	wglMakeCurrent(*hDC, *hRC);

	/*// Enable v-sync.
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT(1);*/

}

// Disable OpenGL

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);
}

/*void UpdateMouse()
{
	IEngine* engine = GetEngine();
	IMouse* mouse = engine->GetMouse();
	ILogger* logger = engine->GetLogger();

	float halfScreenWidth = (float)GetSystemMetrics(SM_CXSCREEN) / 2.0f;
	float halfScreenHeight = (float)GetSystemMetrics(SM_CYSCREEN) / 2.0f;

	POINT cursorPos;
	GetCursorPos(&cursorPos);

	Vec2 movementOffset;
	movementOffset.x = (float)cursorPos.x - halfScreenWidth;
	movementOffset.y = (float)cursorPos.y - halfScreenHeight;
	mouse->RaiseMouseMoveEvent(&movementOffset);

	SetCursorPos((int)halfScreenWidth, (int)halfScreenHeight);
}*/