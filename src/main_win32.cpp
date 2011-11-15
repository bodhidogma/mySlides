// win-slides.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Resource.h"
#include "main_window.h"

#define MAX_LOADSTRING 100

// Global Variables:
AppWindow myWindow;

/**
*/
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// register window class
	myWindow.Register(hInstance);

	// Perform application initialization:
	if (!myWindow.InitInstance (hInstance, nCmdShow))
		return FALSE;

	// run message pump
	return myWindow.MessagePump();
}

/**  AppWindow Class Implementation
*/

AppWindow::AppWindow()
{
}

AppWindow::~AppWindow()
{
}

/**
*/
int AppWindow::Register(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = {0};
	TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
	LoadString(hInstance, IDC_MYSLIDES, szWindowClass, MAX_LOADSTRING);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= this->staticWindProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(ID_APP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= szWindowClass;
//	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MYSLIDES);
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

/**
*/
BOOL AppWindow::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
	LoadString(hInstance, IDC_MYSLIDES, szWindowClass, MAX_LOADSTRING);

	//   hInst = hInstance; // Store instance handle in our global variable

	hWnd = ::CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return FALSE;

	// save "this" ptr for static callback access
	::SetWindowLong( hWnd, GWL_USERDATA, (long)this );

	::ShowWindow(hWnd, nCmdShow);
	::UpdateWindow(hWnd);

	return TRUE;
}

/**
*/
int AppWindow::MessagePump()
{
	MSG msg;
	// Main message loop:
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

/** static callback for all instances of the class
*/
LRESULT CALLBACK AppWindow::staticWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;

	// get handle to original creating class (this)
	AppWindow *aw = (AppWindow*)::GetWindowLong( hWnd, GWL_USERDATA );

	// call "this" callback
	if (aw) aw->AppWindProc(hWnd,message,wParam,lParam);

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

/** instance specific callback
*/
LRESULT AppWindow::AppWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
#if 0
		int wmId    = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDD_ABOUTBOX:
//			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, (DLGPROC)About);
			break;
//		case IDM_EXIT:
//			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
#endif
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
