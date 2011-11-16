// win-slides.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Resource.h"
#include "main_window.h"

// Global Variables:
AppWindow myWindow;

/**
*/
int startWindowedApp(HINSTANCE hInstance, int nCmdShow)
{
	// register window class
	myWindow.RegisterWindow(hInstance);

	// Perform application initialization:
	if (!myWindow.InitInstance(nCmdShow))
		return FALSE;

	// run message pump
	return myWindow.MessagePump();
}

/**
*/
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	return startWindowedApp(hInstance, nCmdShow);
}

/**  AppWindow Class Implementation
*/

AppWindow::AppWindow()
{
	ZeroMemory(&window, sizeof(_Window));
	app = NULL;
}

AppWindow::~AppWindow()
{
	if (app)
		delete app;
}

/**
*/
BOOL AppWindow::RegisterWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = {0};

	// no associated app for this window
	if (this->app == NULL) {
		this->app = new Application;
	}

	this->app->hInstance = hInstance;
	LoadString(hInstance, IDC_MYSLIDES, this->app->className, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, this->window.init.title, MAX_LOADSTRING);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= this->staticWindProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= this->app->hInstance;
	wcex.hIcon			= LoadIcon(this->app->hInstance, MAKEINTRESOURCE(ID_APP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= this->app->className;
//	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MYSLIDES);
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// RegisterClass is supposed to succeed
	if (RegisterClassEx(&wcex) == 0)
	{
		MessageBox (HWND_DESKTOP, _T("RegisterClassEx Failed!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

/**
*/
BOOL AppWindow::InitInstance(int nCmdShow)
{
	// create the window
	this->window.hWnd = ::CreateWindowEx(
		WS_EX_APPWINDOW,			// extended style
		this->app->className,
		this->window.init.title,
		WS_OVERLAPPEDWINDOW,		// window style
		0, 0,						// origin X,Y
		640, 480,					// width,height
		HWND_DESKTOP,				// parent
		NULL,						// menu
		this->app->hInstance,		// window instance
		NULL);						// WM_CREATE message WPARAM value

	if (!this->window.hWnd)
		return FALSE;

	// save "this" ptr for static callback access
	::SetWindowLong( this->window.hWnd, GWL_USERDATA, (long)this );

	// show and update window
	::ShowWindow(this->window.hWnd, nCmdShow);
	::UpdateWindow(this->window.hWnd);				// send WM_PAINT

	return TRUE;
}

/**
*/
int AppWindow::MessagePump()
{
	MSG msg = {0};

	// Main message loop:
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

/** instance specific callback
*/
LRESULT AppWindow::AppWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT r;

			GetClientRect(hWnd,&r);
			hdc = BeginPaint(hWnd, &ps);
			HBRUSH br = (HBRUSH)GetStockObject(BLACK_BRUSH);
			FillRect(hdc,&r,br);
			EndPaint(hWnd, &ps);
		}
		break;
	}
	return 0;
}

/** static callback for all instances of the class
*/
LRESULT CALLBACK AppWindow::staticWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;

	// get handle to original creating class (this) and call class custom callback
	AppWindow *aw = (AppWindow*)::GetWindowLong( hWnd, GWL_USERDATA );
	if (aw) aw->AppWindProc(hWnd,message,wParam,lParam);

	// common message processing functions
	switch (message)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
