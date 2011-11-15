/**
*/

#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

typedef struct {									// Contains Information Vital To Applications
	HINSTANCE		hInstance;						// Application Instance
	TCHAR			className[100];					// Application ClassName
} _Application;										// Application

typedef struct {									// Window Creation Info
	_Application*		application;				// Application Structure
	TCHAR				title[100];				// Window Title
	int					width;						// Width
	int					height;						// Height
	int					bitsPerPixel;				// Bits Per Pixel
	BOOL				isFullScreen;				// FullScreen?
} _WindowInit;									// GL_WindowInit

typedef struct {									// Contains Information Vital To A Window
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context
	_WindowInit		init;						// Window Init
	BOOL				isVisible;					// Window Visible?
	DWORD				lastTickCount;				// Tick Counter
} _Window;										// GL_Window

/** Simple Class to handle some window functions
*/
class AppWindow
{
public:
	AppWindow();
	~AppWindow();
	
	int Register(HINSTANCE);
	BOOL InitInstance(HINSTANCE, int);
	static LRESULT CALLBACK staticWindProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT AppWindProc(HWND, UINT, WPARAM, LPARAM);

	int MessagePump();
private:
};

#endif