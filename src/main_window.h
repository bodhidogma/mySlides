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

// externally defined class
class SlideSaver;

/** Simple Class to handle some window functions
*/
class AppWindow
{
public:
	AppWindow();
	~AppWindow();
	
	void    initFullScreen(BOOL doFS) {window.init.isFullScreen = doFS;}
	void	initSize(int w, int h){window.init.width=w; window.init.height=h;}

	BOOL	registerWindow(HINSTANCE hInstance);
	BOOL	createWindow(int nCmdShow);
	int		messagePump();
	LRESULT	appWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	SlideSaver *saver;
private:
	Application *app;
	_Window window;

	// static callback w/ core functionality
	static LRESULT CALLBACK staticWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif