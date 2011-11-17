// win-slides.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Resource.h"
#include "mySlideSaver.h"
//#include "main_window.h"

/**
*/
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

//	AppWindow *mySaver = new AppWindow();
	SlideSaver *mySaver = new SlideSaver();
	int ret = 0;
	LPWSTR *szArglist;
	int nArgs;
	int i;

	if (!mySaver)
		return -1;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	for (i=1; szArglist && i<nArgs; i++) {
		ret = i;
		// win95 change password
		if (!_tcsnicmp(_T("/a"), szArglist[i],2)) {
			ret = -1;
			break;
		}
		// saver settings dialog
		else if (!_tcsnicmp(_T("/c"), szArglist[i],2)) {
			// open config box
			ret = 0;
			break;
		}
		// open preview window
		else if (!_tcsnicmp(_T("/p"), szArglist[i],2)) {
			if ((i+1)<nArgs)
				mySaver->setParent((HWND)_tstoi(szArglist[i+1]));
			else 
				ret = -1;
			break;
		}
		// start saver
		else if (!_tcsnicmp(_T("/s"), szArglist[i],2)) {
			mySaver->setFullScreen(TRUE);
			break;
		}
		// windowed saver (debugging)
		else if (!_tcsnicmp(_T("/w"), szArglist[i],2)) {
			mySaver->setSize(640,480);
			break;
		}
	}
	switch (ret)
	{
	case -1:	// some errors
		break;
	case 0:		// open configure dialog
		break;
	default:	// open window and start msg pumps
		ret = mySaver->startApp(hInstance, nCmdShow);
	}
	delete mySaver;
	mySaver = 0;

	LocalFree(szArglist);
	return ret;
}

/**  AppWindow Class Implementation
*/

AppWindow::AppWindow()
{
	ZeroMemory(&window, sizeof(_Window));
	app = NULL;

	window.init.width = 320;
	window.init.height = 240;
}

AppWindow::~AppWindow()
{
	if (app) delete app;
}

/**
*/
int AppWindow::startApp(HINSTANCE hInstance, int nCmdShow)
{
	// register window class
	registerWindow(hInstance);

	// Perform application initialization:
	if (!createWindow(nCmdShow))
		return FALSE;

	// run message pump
	return messagePump();
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

	if (this->window.hParent) {
	}
	else if (this->window.init.isFullScreen) {
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

	this->window.hDC = GetDC(this->window.hWnd);

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
			this->idleProc();
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

/**
*/
void AppWindow::setBestPixelFormat()
{
	int moreFormats, score = 0, nPixelFormat = 1, bestPixelFormat = 0, temp;
	PIXELFORMATDESCRIPTOR pfd;

	// Try to find the best pixel format
	moreFormats = DescribePixelFormat(this->window.hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	while(moreFormats){
		// Absolutely must have these 4 attributes
		if((pfd.dwFlags & PFD_SUPPORT_OPENGL)
			&& (pfd.dwFlags & PFD_DRAW_TO_WINDOW)
			&& (pfd.dwFlags & PFD_DOUBLEBUFFER)
			&& (pfd.iPixelType == PFD_TYPE_RGBA)){
			// If this pixel format is good, see if it's the best...
			temp = 0;
			// color depth and z depth?
			temp += pfd.cColorBits + 2 * pfd.cDepthBits;
			if(pfd.cColorBits > 16)
				temp += (16 - pfd.cColorBits) / 2;
			// hardware accelerated?
			if(pfd.dwFlags & PFD_GENERIC_FORMAT){
				if(pfd.dwFlags & PFD_GENERIC_ACCELERATED)
					temp += 1000;
			}
			else
				temp += 2000;
			// Compare score
			if(temp > score){
				score = temp;
				bestPixelFormat = nPixelFormat;
			}
		}
		// Try the next pixel format
		nPixelFormat++;
		moreFormats = DescribePixelFormat(this->window.hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	}

	// Set the pixel format for the device con_T
	if(bestPixelFormat){
		DescribePixelFormat(this->window.hDC, bestPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		SetPixelFormat(this->window.hDC, bestPixelFormat, &pfd);
#if 0
		if(pfd.dwFlags & PFD_SWAP_EXCHANGE)
			pfd_swap_exchange = 1;
		else
			pfd_swap_exchange = 0;
		if(pfd.dwFlags & PFD_SWAP_COPY)
			pfd_swap_copy = 1;
		else
			pfd_swap_copy = 0;
#endif
	}
	else{  // Just in case a best pixel format wasn't found
		PIXELFORMATDESCRIPTOR defaultPfd = {
			sizeof(PIXELFORMATDESCRIPTOR),          // Size of this structure
			1,                                      // Version of this structure    
			PFD_DRAW_TO_WINDOW |                    // Draw to Window (not to bitmap)
			PFD_SUPPORT_OPENGL |					// Support OpenGL calls in window
			PFD_DOUBLEBUFFER,                       // Double buffered
			PFD_TYPE_RGBA,                          // RGBA Color mode
			24,                                     // Want 24bit color 
			0,0,0,0,0,0,                            // Not used to select mode
			0,0,                                    // Not used to select mode
			0,0,0,0,0,                              // Not used to select mode
			24,                                     // Size of depth buffer
			0,                                      // Not used to select mode
			0,                                      // Not used to select mode
			PFD_MAIN_PLANE,                         // Draw in main plane
			0,                                      // Not used to select mode
			0,0,0 };                                // Not used to select mode
		bestPixelFormat = ChoosePixelFormat(this->window.hDC, &defaultPfd);
		// ChoosePixelFormat is poorly documented and I don't trust it.
		// That's the main reason for this whole function.
		SetPixelFormat(this->window.hDC, bestPixelFormat, &defaultPfd);
#if 0
		if(defaultPfd.dwFlags & PFD_SWAP_EXCHANGE)
			pfd_swap_exchange = 1;
		else
			pfd_swap_exchange = 0;
		if(defaultPfd.dwFlags & PFD_SWAP_COPY)
			pfd_swap_copy = 1;
		else
			pfd_swap_copy = 0;
#endif
	}
}

void AppWindow::_initSaver()
{

}