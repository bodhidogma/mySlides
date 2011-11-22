// win-slides.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Resource.h"
#include "mySlideSaver.h"
//#include "main_window.h"

#include "rsTimer.h"
#include <fstream>

BOOL checkPassword(HWND hwnd);
void changePassword(HWND hwnd);

/**
*/
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
#if 0
	std::ofstream outfile;
	outfile.open("c:\\tmp\\cmdline"); outfile<<"cmd="<< GetCommandLineA() <<std::endl; outfile.close();
#endif

	//AppWindow *mySaver = {0};
	SlideSaver *mySaver = {0};

	mySaver = new SlideSaver();
	int ret = 0;
	LPWSTR *szArglist;
	int nArgs;
	int i;
	HWND hParent = HWND_DESKTOP;

	if (!mySaver)
		return -1;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	for (i=1; szArglist && i<nArgs; i++) {
		ret = i;
		// win95 change password
		if (!_wcsnicmp(L"/a", szArglist[i],2)) {
			ret = -1;
			break;
		}
		// saver settings dialog
		else if (!_wcsnicmp(L"/c", szArglist[i],2)) {
			hParent = GetForegroundWindow();
			mySaver->setParent(hParent);
			ret = 0;
			break;
		}
		// open preview window
		else if (!_wcsnicmp(L"/p", szArglist[i],2)) {
			if ((i+1)<nArgs) {
				hParent = (HWND)_wtoi(szArglist[i+1]);
				mySaver->setParent(hParent);
			}
			else 
				ret = -1;
			break;
		}
		// start saver
		else if (!_wcsnicmp(L"/s", szArglist[i],2)) {
			mySaver->setFullScreen(TRUE);
			break;
		}
		// windowed saver (debugging)
		else if (!wcsncmp(L"/w", szArglist[i],2)) {
			mySaver->setSize(640,480);
			break;
		}
	}
	LocalFree(szArglist);

	switch (ret)
	{
	case -1:	// some errors
		break;
	case 0:		// open configure dialog
		ret = mySaver->openConfigBox(hParent, hInstance);
		break;
	default:	// open window and start msg pumps
		ret = mySaver->startApp(hParent, hInstance, nCmdShow);
	}
	delete mySaver;

	return ret;
}

// --------------------------------------------------------------

/**  AppWindow Class Implementation
*/

AppWindow::AppWindow()
{
	memset(&window,0,sizeof(_Window));
	app = NULL;

	window.init.width = 320;
	window.init.height = 240;
	window.init.screenW = GetSystemMetrics(SM_CXVIRTUALSCREEN)-GetSystemMetrics(SM_XVIRTUALSCREEN);
	window.init.screenH = GetSystemMetrics(SM_CYVIRTUALSCREEN)-GetSystemMetrics(SM_YVIRTUALSCREEN);
}

AppWindow::~AppWindow()
{
	if (app) delete app;
}

/**
*/
int AppWindow::startApp(HWND hParent, HINSTANCE hInstance, int nCmdShow)
{
	// Register && create window instance:
	if (registerWindow(hInstance) 
		&& createWindow(hParent, hInstance, nCmdShow))
	{
		// run message pump
		return messagePump();
	}
	return FALSE;
}

/**
*/
int AppWindow::openConfigBox(HWND hParent, HINSTANCE inst)
{
	DialogBoxParam(inst,
		MAKEINTRESOURCE(DLG_SCRNSAVECONFIGURE),
		hParent,
		(DLGPROC)this->_staticDialogProc,
		(LPARAM)this
		);
	return 0;
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
	wcex.lpfnWndProc	= this->_staticWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(ID_APP));
//	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);				// default BG color
	wcex.hbrBackground	= HBRUSH(GetStockObject(BLACK_BRUSH));	// default BG color
//	wcex.hbrBackground	= NULL;
	wcex.lpszClassName	= this->app->className;
	wcex.lpszMenuName	= NULL;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// look for another app instance and switch to it
	HWND hOther = FindWindow(this->app->className, this->window.init.title);
	if (hOther && IsWindow(hOther)) {
		SetForegroundWindow(hOther);
		return FALSE;
	}

	// RegisterClass is supposed to succeed
	if (RegisterClassEx(&wcex) == 0) {
		MessageBoxW(HWND_DESKTOP, L"RegisterClassEx Failed!", L"Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

/**
*/
BOOL AppWindow::createWindow(HWND hParent, HINSTANCE hInstance, int nCmdShow)
{
	UINT exStyle = WS_EX_APPWINDOW;
	UINT style = WS_OVERLAPPEDWINDOW;

	int left, top, width, height;
	left = top = width = height = 0;

	// if parent set, make sure not trying to do full screen
	if (hParent && !IsWindow(hParent))
		return FALSE;

	// preview
	if (hParent) {
		RECT pRect;
		GetClientRect(hParent, &pRect);
		width = pRect.right;
		height = pRect.bottom;
		
		style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
		exStyle = 0;
		window.init.isPreview = TRUE;

		// set different name for init.title
		LoadString(hInstance, IDS_APP_TITLE_PVW, window.init.title, MAX_LOADSTRING);
	}
	// full screen
	else if (window.init.isFullScreen) {
		left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		top = GetSystemMetrics(SM_YVIRTUALSCREEN);
		width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	
		style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exStyle = WS_EX_TOPMOST;
		hParent = HWND_DESKTOP;
	}
	// windowed
	else {
		width = this->window.init.width;
		height = this->window.init.height;
	}
	window.hParent = hParent;

	// create the window
	this->window.hWnd = ::CreateWindowEx(
		exStyle,					// extended style
		this->app->className,
		this->window.init.title,
		style,						// window style
		left, top,					// origin X,Y
		width, height,				// width,height
		hParent,					// parent
		NULL,						// menu
		hInstance,					// window instance
		(LPVOID)this				// push (this) to be associated with the window
		);

	if (!this->window.hWnd)
		return FALSE;

	if (!this->window.init.isPreview)
		::SetForegroundWindow( this->window.hWnd );

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

	float timeRemaining = 0.0f;
	rsTimer timer;

	while (1) {
		// Main message loop:
		while (!::PeekMessage(&msg, NULL,0,0, PM_NOREMOVE)) {

			if (window.isSuspended)
				Sleep(1);
			else if (window.timeStep != 0.0f) {
				timeRemaining -= timer.tick();
				// prevent underflow
				if (timeRemaining < -1000.0f)
					timeRemaining = 0.0f;
				if (timeRemaining> 0.0f) {
					// wait some more
					if (timeRemaining > 0.001f)
						Sleep(1);
				}
				else {
					this->idleProc();
					timeRemaining += window.timeStep;
				}
			}
			else {
				this->idleProc();
				Sleep(1);
			}
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
LRESULT AppWindow::appWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		this->window.hWnd = hWnd;
		this->window.hDC = GetDC(hWnd);
		this->initSaver();
		break;
	case WM_DESTROY:
		this->cleanUp();
		break;
	case WM_SIZE:
		this->shapeWindow();
		break;
	case WM_KEYDOWN:
		break;
	case WM_SETCURSOR:
		if (!window.init.isPreview && window.init.isFullScreen)
			SetCursor(NULL);		// clear cursor if full screen
		break;
	case WM_SYSCOMMAND:
		if (!window.init.isPreview) {
			switch (wParam) 
			{
			case SC_NEXTWINDOW:
			case SC_PREVWINDOW:
			case SC_SCREENSAVE:
				return FALSE;
			case SC_MONITORPOWER:
				if (lParam == 1 || lParam == 2 && window.init.isFullScreen)
					window.isSuspended = 1;
				break;
			}
		}
	case WM_CLOSE:
		window.isSuspended = 0;
#if 0
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
#endif
	}
	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

/** static callback for all instances of the class
*/
LRESULT CALLBACK AppWindow::_staticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// get handle to original creating class (this) and call class custom callback
	AppWindow *aw = (AppWindow*)::GetWindowLong( hWnd, GWLP_USERDATA );
	// common message processing
	switch (message) {
	case WM_NCCREATE:
		if (!aw) {
			aw = (AppWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
			SetWindowLong(hWnd, GWLP_USERDATA, (long)(aw));
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	}
	if (aw)
		return aw->appWindowProc(hWnd,message,wParam,lParam);
	else
		return ::DefWindowProc(hWnd, message, wParam, lParam);
}

/**
*/
INT_PTR CALLBACK AppWindow::_staticDialogProc(HWND hDlg, UINT msg, WPARAM wpm, LPARAM lpm)
{
	// get handle to original creating class (this) and call class custom callback
	AppWindow *aw = (AppWindow*)::GetWindowLong( hDlg, GWLP_USERDATA );
	// common message processing
	switch(msg) {
	case WM_INITDIALOG:
		InitCommonControls();
		if (!aw) {
			aw = (AppWindow*)lpm;
			SetWindowLongPtr(hDlg, GWLP_USERDATA, lpm);
		}
		break;
#if 0
	case WM_COMMAND:
        switch(LOWORD(wpm)){
        case IDOK:
            // Fall through
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wpm));
            break;
		}
        return TRUE;
#endif
	}
	if (aw)
		return aw->saverConfigureDialog(hDlg, msg, wpm, lpm);
	else
		return FALSE;
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
