/**
*/

#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

typedef struct {									// Contains Information Vital To Applications
	HINSTANCE		hInstance;						// Application Instance
	TCHAR			className[MAX_LOADSTRING];		// Application ClassName
} Application;										// Application

typedef struct {									// Window Creation Info
	TCHAR				title[MAX_LOADSTRING];		// Window Title
	int					width;						// Width
	int					height;						// Height
	int					bitsPerPixel;				// Bits Per Pixel
	BOOL				isFullScreen;				// FullScreen?
	BOOL				isPreview;
} _WindowInit;									// GL_WindowInit

typedef struct {									// Contains Information Vital To A Window
	HWND				hParent;
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
	
	void    setFullScreen(BOOL doFS) {window.init.isFullScreen = doFS;}
	void	setSize(int w, int h){window.init.width=w; window.init.height=h;}
	void	setParent(HWND hWnd) {window.hParent=hWnd; }

	int     startApp(HINSTANCE hInstance, int nCmdShow);

	BOOL	registerWindow(HINSTANCE hInstance);
	BOOL	createWindow(int nCmdShow);
	int		messagePump();
	LRESULT	appWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	Application *app;
	_Window window;

	void _initSaver();

	// Abstract base class function declarations
	virtual int  idleProc() =0;
	virtual void shapeWindow() =0;
	virtual void initSaver() =0;
	virtual void cleanUp() =0;
	virtual BOOL saverConfigureDialog(HWND hDlg, UINT msg, WPARAM wpm, LPARAM lpm) =0;

private:
	// static callback w/ core functionality
	static LRESULT CALLBACK staticWindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void setBestPixelFormat();
};

#endif