/**
*/

#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

typedef struct {									// Contains Information Vital To Applications
	HINSTANCE		hInstance;						// Application Instance
	TCHAR			className[MAX_LOADSTRING];		// Application ClassName
} Application;										// Application

typedef struct {									// Window Creation Info
//	Application*		application;				// Application Structure
	TCHAR				title[MAX_LOADSTRING];		// Window Title
	int					width;						// Width
	int					height;						// Height
	int					bitsPerPixel;				// Bits Per Pixel
	BOOL				isFullScreen;				// FullScreen?
} _WindowInit;									// GL_WindowInit

typedef struct {									// Contains Information Vital To A Window
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context
	_WindowInit			init;						// Window Init
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
	
	BOOL	RegisterWindow(HINSTANCE hInstance);
	BOOL	InitInstance(int nCmdShow);
	int		MessagePump();
	LRESULT	AppWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	Application *app;
	_Window window;

	// static callback w/ core functionality
	static LRESULT CALLBACK staticWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif