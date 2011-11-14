// win-slides.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "mySlides.h"
#include "mySlideShow.h"

#define MAX_LOADSTRING 100
#define WM_TOGGLEFULLSCREEN (WM_USER+1)

static BOOL g_isProgramLooping;
static BOOL g_createFullScreen;

#define INIT_WIN_W	640
#define INIT_WIN_H	400

#define SLEEP_TIME	100

/**
*/
void TerminateApplication (GL_Window* window)
{
	PostMessage (window->hWnd, WM_QUIT, 0, 0);
	g_isProgramLooping = FALSE;
}

/**
*/
void ToggleFullscreen (GL_Window* window)
{
	PostMessage (window->hWnd, WM_TOGGLEFULLSCREEN, 0, 0);
}

/**
*/
void ReshapeGL (int width, int height)
{
	glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));		// Reset The Current Viewport
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();											// Reset The Projection Matrix
#if 0
	gluPerspective (45.0f, (GLfloat)(width)/(GLfloat)(height),
					1.0f, 100.0f);
#else
//	GLfloat zNear = 1.0f;
//	GLfloat zFar = 1000.0f;
	GLfloat aspect = float(width)/float(height);
//	GLfloat fH = (float)tan( float(90.0 / 360.0f * 3.14159f) ) * zNear;
//	GLfloat fW = fH * aspect;
	
	glOrtho(-1*aspect,1*aspect, -1,1, -1,100);
#endif
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();											// Reset The Modelview Matrix

	WindowWasResized( width, height );
}

/**
*/
BOOL ChangeScreenResolution (int width, int height, int bitsPerPixel)
{
	DEVMODE dmScreenSettings;											// Device Mode
	ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));
	dmScreenSettings.dmSize				= sizeof (DEVMODE);
	dmScreenSettings.dmPelsWidth		= width;
	dmScreenSettings.dmPelsHeight		= height;
	dmScreenSettings.dmBitsPerPel		= bitsPerPixel;
	dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return FALSE;
	}
	return TRUE;
}


// Choose the best pixel format possible, giving preference to harware
// accelerated modes.
void setBestPixelFormat(HDC hdc){
	int moreFormats, score = 0, nPixelFormat = 1, bestPixelFormat = 0, temp;
	PIXELFORMATDESCRIPTOR pfd;

	// global?
	int pfd_swap_exchange = 0;
	int pfd_swap_copy = 0;

	// Try to find the best pixel format
	moreFormats = DescribePixelFormat(hdc, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
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
		moreFormats = DescribePixelFormat(hdc, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	}

	// Set the pixel format for the device context
	if(bestPixelFormat){
		DescribePixelFormat(hdc, bestPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		SetPixelFormat(hdc, bestPixelFormat, &pfd);
		if(pfd.dwFlags & PFD_SWAP_EXCHANGE)
			pfd_swap_exchange = 1;
		else
			pfd_swap_exchange = 0;
		if(pfd.dwFlags & PFD_SWAP_COPY)
			pfd_swap_copy = 1;
		else
			pfd_swap_copy = 0;
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
		bestPixelFormat = ChoosePixelFormat(hdc, &defaultPfd);
		// ChoosePixelFormat is poorly documented and I don't trust it.
		// That's the main reason for this whole function.
		SetPixelFormat(hdc, bestPixelFormat, &defaultPfd);
		if(defaultPfd.dwFlags & PFD_SWAP_EXCHANGE)
			pfd_swap_exchange = 1;
		else
			pfd_swap_exchange = 0;
		if(defaultPfd.dwFlags & PFD_SWAP_COPY)
			pfd_swap_copy = 1;
		else
			pfd_swap_copy = 0;
	}
}


/**
*/
BOOL CreateWindowGL (GL_Window* window)
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;

	RECT windowRect = {0, 0, window->init.width, window->init.height};

	GLuint PixelFormat;

	if (window->init.isFullScreen == TRUE)
	{
		if (ChangeScreenResolution (window->init.width, window->init.height, window->init.bitsPerPixel) == FALSE)
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			MessageBox (HWND_DESKTOP, _T("Mode Switch Failed.\nRunning In Windowed Mode."), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
			window->init.isFullScreen = FALSE;
		}
		else
		{
			ShowCursor (FALSE);
			windowStyle = WS_POPUP;
			windowExtendedStyle |= WS_EX_TOPMOST;
		}
	}
	else
	{
		// Adjust Window, Account For Window Borders
		AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// Create The OpenGL Window
	window->hWnd = CreateWindowEx (windowExtendedStyle,					// Extended Style
								   window->init.application->className,	// Class Name
								   window->init.title,					// Window Title
								   windowStyle,							// Window Style
								   0, 0,								// Window X,Y Position
								   windowRect.right - windowRect.left,	// Window Width
								   windowRect.bottom - windowRect.top,	// Window Height
								   HWND_DESKTOP,						// Desktop Is Window's Parent
								   0,									// No Menu
								   window->init.application->hInstance, // Pass The Window Instance
								   window);

	if (window->hWnd == 0)
	{
		return FALSE;
	}

	window->hDC = GetDC (window->hWnd);
	if (window->hDC == 0)
	{
		// Failed
		DestroyWindow (window->hWnd);
		window->hWnd = 0;
	}
#if 0
	PIXELFORMATDESCRIPTOR pfd =									// pfd Tells Windows How We Want Things To Be
	{
		sizeof (PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |									// Format Must Support Window
		PFD_SUPPORT_OPENGL |									// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,										// Must Support Double Buffering
		PFD_TYPE_RGBA,											// Request An RGBA Format
		window->init.bitsPerPixel,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,										// Color Bits Ignored
		0,														// No Alpha Buffer
		0,														// Shift Bit Ignored
		0,														// No Accumulation Buffer
		0, 0, 0, 0,												// Accumulation Bits Ignored
		16,														// 16Bit Z-Buffer (Depth Buffer)  
		0,														// No Stencil Buffer
		0,														// No Auxiliary Buffer
		PFD_MAIN_PLANE,											// Main Drawing Layer
		0,														// Reserved
		0, 0, 0													// Layer Masks Ignored
	};

	PixelFormat = ChoosePixelFormat (window->hDC, &pfd);	// Find A Compatible Pixel Format
	if (PixelFormat == 0)
	{
		// Failed
		ReleaseDC (window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow (window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	if (SetPixelFormat (window->hDC, PixelFormat, &pfd) == FALSE)	// Try To Set The Pixel Format
	{
		// Failed
		ReleaseDC (window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow (window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}
#else
	setBestPixelFormat(window->hDC);
#endif
	window->hRC = wglCreateContext (window->hDC);	// Try To Get A Rendering Context
	if (window->hRC == 0)
	{
		// Failed
		ReleaseDC (window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow (window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	// Make The Rendering Context Our Current Rendering Context
	if (wglMakeCurrent (window->hDC, window->hRC) == FALSE)
	{
		// Failed
		wglDeleteContext (window->hRC);
		window->hRC = 0;
		ReleaseDC (window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow (window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	ShowWindow (window->hWnd, SW_NORMAL);
	window->isVisible = TRUE;

	ReshapeGL (window->init.width, window->init.height);

	ZeroMemory (window->keys, sizeof (Keys));

	window->lastTickCount = GetTickCount ();

	return TRUE;
}

/**
*/
BOOL DestroyWindowGL (GL_Window* window)
{
	if (window->hWnd != 0)
	{	
		if (window->hDC != 0)
		{
			wglMakeCurrent (window->hDC, 0);	// Set The Current Active Rendering Context To Zero
			if (window->hRC != 0)
			{
				wglDeleteContext (window->hRC);
				window->hRC = 0;

			}
			ReleaseDC (window->hWnd, window->hDC);
			window->hDC = 0;
		}
		DestroyWindow (window->hWnd);
		window->hWnd = 0;
	}

	if (window->init.isFullScreen)
	{
		ChangeDisplaySettings (NULL,0);
		ShowCursor (TRUE);
	}	
	return TRUE;
}

/** Process Window Message Callbacks
*/
LRESULT CALLBACK WindowProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Get The Window Context
	GL_Window* window = (GL_Window*)(GetWindowLong (hWnd, GWL_USERDATA));

	switch (uMsg)
	{
		case WM_SYSCOMMAND:		// Intercept System Commands
		{
			switch (wParam)		// Check System Calls
			{
				case SC_SCREENSAVE:		// Screensaver Trying To Start?
				case SC_MONITORPOWER:	// Monitor Trying To Enter Powersave?
				return 0;
			}
			break;
		}
		return 0;

		case WM_CREATE:
		{
			CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);	// Store Window Structure Pointer
			window = (GL_Window*)(creation->lpCreateParams);
			SetWindowLong (hWnd, GWL_USERDATA, (LONG)(window));
		}
		return 0;

		case WM_CLOSE:
			TerminateApplication(window);
		return 0;

		case WM_SIZE:
			switch (wParam)				// Evaluate Size Action
			{
				case SIZE_MINIMIZED:
					window->isVisible = FALSE;
				return 0;

				case SIZE_MAXIMIZED:	// Was Window Maximized?
					window->isVisible = TRUE;
					ReshapeGL (LOWORD (lParam), HIWORD (lParam));
				return 0;

				case SIZE_RESTORED:		// Was Window Restored?
					window->isVisible = TRUE;
					ReshapeGL (LOWORD (lParam), HIWORD (lParam));
				return 0;
			}
		break;

		case WM_TIMER:
			Timer(window);
			break;

		case WM_KEYDOWN:	// http://msdn.microsoft.com/en-us/library/ms927178.aspx
			if ((wParam >= 0) && (wParam <= 255))
			{
				window->keys->keyDown [wParam] = TRUE;	// Set The Selected Key (wParam) To True
				return 0;
			}
		break;

		case WM_KEYUP:
			if ((wParam >= 0) && (wParam <= 255))
			{
				window->keys->keyDown [wParam] = FALSE;
				return 0;
			}
		break;

		case WM_TOGGLEFULLSCREEN:
			g_createFullScreen = (g_createFullScreen == TRUE) ? FALSE : TRUE;
			PostMessage (hWnd, WM_QUIT, 0, 0);
		break;

//		case WM_PAINT:
		case WM_ERASEBKGND:
//			return TRUE;
			break;
	}

	return DefWindowProc (hWnd, uMsg, wParam, lParam);
}

/**
*/
BOOL RegisterWindowClass (Application* application)
{
	// Register A Window Class
	WNDCLASSEX windowClass;
	ZeroMemory (&windowClass, sizeof (WNDCLASSEX));
	windowClass.cbSize			= sizeof (WNDCLASSEX);
	
	windowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraws The Window For Any Movement / Resizing
	windowClass.lpfnWndProc		= (WNDPROC)(WindowProc);
	windowClass.cbClsExtra		= 0;
	windowClass.cbWndExtra		= 0;
	windowClass.hInstance		= application->hInstance;
	windowClass.hIcon			= LoadIcon(application->hInstance, MAKEINTRESOURCE(IDI_MYSLIDES));
	windowClass.hIconSm			= LoadIcon(application->hInstance, MAKEINTRESOURCE(IDI_MYSLIDES));
	windowClass.hbrBackground	= (HBRUSH)COLOR_WINDOW+1;
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszMenuName	= 0;
	windowClass.lpszClassName	= application->className;
	if (RegisterClassEx (&windowClass) == 0)
	{
		// NOTE: Failure, Should Never Happen
		MessageBox (HWND_DESKTOP, _T("RegisterClassEx Failed!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

/** Program Entry (WinMain)
*/
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application			application = {0};
	GL_Window			window = {0};
	Keys				keys = {0};
	BOOL				isMessagePumpActive;
	MSG					msg;
	DWORD				tickCount;


	// Fill Out Application Data
	application.hInstance = hInstance;
	LoadString(hInstance, IDC_MYSLIDES, application.className, MAX_LOADSTRING);

	// Fill Out Window
	ZeroMemory (&window, sizeof (GL_Window));
	window.keys					= &keys;
	window.init.application		= &application;
	LoadString(hInstance, IDS_APP_TITLE, window.init.title, MAX_LOADSTRING);
	window.init.width			= INIT_WIN_W;
	window.init.height			= INIT_WIN_H;
	window.init.bitsPerPixel	= 32;
	window.init.isFullScreen	= FALSE;

	ZeroMemory (&keys, sizeof (Keys));

	// Ask The User If They Want To Start In FullScreen Mode?
#if 0
	if (MessageBox (HWND_DESKTOP, _T("Would You Like To Run In Fullscreen Mode?"),
		_T("Start FullScreen?"),MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		window.init.isFullScreen = FALSE;
	}
	else
		window.init.isFullScreen = TRUE;
#endif

	// Register A Class For Our Window To Use
	if (RegisterWindowClass (&application) == FALSE)
	{
		return -1;														// Terminate Application
	}

	g_isProgramLooping = TRUE;
	g_createFullScreen = window.init.isFullScreen;
	while (g_isProgramLooping)
	{
		// Create A Window
		window.init.isFullScreen = g_createFullScreen;
		if (CreateWindowGL (&window) == TRUE)
		{
			// At This Point We Should Have A Window That Is Setup To Render OpenGL
			if (Initialize (&window, &keys) == FALSE)
			{
				TerminateApplication (&window);
			}
			else
			{
				isMessagePumpActive = TRUE;
				while (isMessagePumpActive == TRUE)
				{
					if (PeekMessage (&msg, window.hWnd, 0, 0, PM_REMOVE) != 0)
					{
						// Check For WM_QUIT Message
						if (msg.message != WM_QUIT)
						{
							DispatchMessage (&msg);
						}
						else
						{
							isMessagePumpActive = FALSE;
						}
					}
					else
					{
						if (window.isVisible == FALSE)
						{
							WaitMessage ();
						}
						else
						{
							// Process Application Loop
							tickCount = GetTickCount ();
							Update (&window, tickCount - window.lastTickCount);
							window.lastTickCount = tickCount;

							Draw (&window);	// process drawing
				
							glFlush ();
							SwapBuffers (window.hDC);

							Sleep(SLEEP_TIME);	// delay so other messages can be processed
						}
					}
				}
			}

			// Application Is Finished
			Deinitialize ();
			DestroyWindowGL (&window);
		}
		else
		{
			// Error Creating Window
			MessageBox (HWND_DESKTOP, _T("Error Creating OpenGL Window"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
			g_isProgramLooping = FALSE;
		}
	}

	UnregisterClass (application.className, application.hInstance);		// UnRegister Window Class
	return 0;
}

