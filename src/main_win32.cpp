// win-slides.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Resource.h"
#include "main_window.h"
#include "mySlideSaver.h"

/**
*/
int startApp(AppWindow *aw, HINSTANCE hInstance, int nCmdShow)
{
	// register window class
	aw->registerWindow(hInstance);

	// Perform application initialization:
	if (!aw->createWindow(nCmdShow))
		return FALSE;

	// run message pump
	return aw->messagePump();
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

	AppWindow *myAppWin = new AppWindow();
	SlideSaver *mySaver = new SlideSaver();
	int ret = 0;

	if (myAppWin) {
//		myAppWin->initFullScreen(TRUE);
//		myAppWin->initSize(640,480);
		myAppWin->saver = mySaver;

		ret = startApp(myAppWin, hInstance, nCmdShow);	
		delete myAppWin;
	}
	if (mySaver) delete mySaver;

	return ret;
}

/**  AppWindow Class Implementation
*/

AppWindow::AppWindow()
{
	ZeroMemory(&window, sizeof(_Window));
	app = NULL;
	saver = NULL;

	window.init.width = 320;
	window.init.height = 240;
}

AppWindow::~AppWindow()
{
	if (app) delete app;
}

/**
*/
BOOL AppWindow::registerWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = {0};

	// no associated app for this window
	if (this->app == NULL) {
		this->app = new Application;
	}

	this->app->hInstance = hInstance;
	LoadString(hInstance, IDC_MYSLIDES, this->app->className, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, this->window.init.title, MAX_LOADSTRING);

	if (this->window.init.isFullScreen) {
		wcex.hCursor = 0;
	}
	else {
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	}

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= this->staticWindProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= this->app->hInstance;
	wcex.hIcon			= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(ID_APP));
//	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);				// default BG color
	wcex.hbrBackground	= HBRUSH(GetStockObject(BLACK_BRUSH));	// default BG color
//	wcex.hbrBackground	= NULL;
	wcex.lpszClassName	= this->app->className;
	wcex.lpszMenuName	= NULL;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// RegisterClass is supposed to succeed
	if (RegisterClassEx(&wcex) == 0) {
		MessageBox (HWND_DESKTOP, _T("RegisterClassEx Failed!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

/**
*/
BOOL AppWindow::createWindow(int nCmdShow)
{
	UINT exStyle = WS_EX_APPWINDOW;
	UINT style = WS_OVERLAPPEDWINDOW;

	int left, top, width, height;

	if (this->window.init.isFullScreen) {
		left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		top = GetSystemMetrics(SM_YVIRTUALSCREEN);
		width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	
		style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exStyle = WS_EX_TOPMOST;
	}
	else {
		left = 0;
		top = 0;
		width = this->window.init.width;
		height = this->window.init.height;
	}

	// create the window
	this->window.hWnd = ::CreateWindowEx(
		exStyle,					// extended style
		this->app->className,
		this->window.init.title,
		style,						// window style
		left, top,					// origin X,Y
		width, height,				// width,height
		HWND_DESKTOP,				// parent
		NULL,						// menu
		this->app->hInstance,		// window instance
		NULL);						// WM_CREATE message WPARAM value

	if (!this->window.hWnd)
		return FALSE;

	// save "this" ptr for static callback access
	::SetWindowLong( this->window.hWnd, GWL_USERDATA, (long)this );

//	if (this->window.init.isFullScreen) {
		::SetForegroundWindow( this->window.hWnd );
//	}

	// show and update window
	::ShowWindow(this->window.hWnd, nCmdShow);
	::UpdateWindow(this->window.hWnd);				// send WM_PAINT

	return TRUE;
}

/**
*/
int AppWindow::messagePump()
{
	MSG msg = {0};

	if (!this->window.hWnd)
		return 0;

	while (1) {
		// Main message loop:
		while (!::PeekMessage(&msg, NULL,0,0, PM_NOREMOVE)) {
			saver->idleProc();
			Sleep(1);
		}
		// process message if found in queue
		if (::GetMessage(&msg, NULL, 0, 0)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
			return msg.wParam;
	}
	return msg.wParam;
}

/** instance specific callback
*/
LRESULT AppWindow::appWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		break;
	case WM_SETCURSOR:
		if (this->window.init.isFullScreen)
			SetCursor(NULL);		// clear cursor if full screen
		return TRUE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT r;

			GetClientRect(hWnd,&r);
			hdc = BeginPaint(hWnd, &ps);
			HBRUSH br = (HBRUSH)GetStockObject(GRAY_BRUSH);
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
	// get handle to original creating class (this) and call class custom callback
	AppWindow *aw = (AppWindow*)::GetWindowLong( hWnd, GWL_USERDATA );
	if (aw) aw->appWindProc(hWnd,message,wParam,lParam);

	// common message processing functions
	switch (message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
