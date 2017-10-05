/**
*/

#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

typedef struct {							// Contains Information Vital To Applications
	HINSTANCE		hInstance;					// Application Instance
	TCHAR			className[MAX_LOADSTRING];	// Application window ClassName
	BOOL			isFullScreen;				// FullScreen?
	BOOL			isPreview;
	int				dispCnt;					// number of displays
	RECT			*dispRect;					// display rectangles
} Application;								// Application

typedef struct {							// Window Creation Info
	TCHAR			title[MAX_LOADSTRING];		// Window Title
	int				width;						// Width
	int				height;						// Height
	int				screenW;
	int				screenH;
	int				bitsPerPixel;				// Bits Per Pixel

//	BOOL				isFullScreen;			// FullScreen?
//	BOOL				isPreview;
} _WindowInit;								// GL_WindowInit

typedef struct {							// Contains Information Vital To A Window
	HWND			hParent;
	HWND			hWnd;						// Window Handle
	HDC				hDC;						// Device Context
	HGLRC			hRC;						// Rendering Context
	_WindowInit		init;						// Window Init
	BOOL			isVisible;					// Window Visible?
	DWORD			lastTickCount;				// Tick Counter
	float			timeStep;
	BOOL			isSuspended;
} _Window;									// GL_Window

// externally defined class
class SlideSaver;

/** Simple Class to handle some window functions
*/
class AppWindow
{
public:
	AppWindow();
	~AppWindow();

	inline void  setFPSLimit(float FPS) {window.timeStep = (FPS ? 1.0f/FPS: 0.0f);}
	inline float getFPSLimit(){ return 1.0f/window.timeStep; };
	
	inline void  setFullScreen(BOOL doFS) {app.isFullScreen = doFS;}
	inline void  setSize(int w, int h){window.init.width=w; window.init.height=h;}
	inline void  setParent(HWND hWnd) {window.hParent=hWnd; }

	int     startApp(HWND hParent, HINSTANCE hInstance, int nCmdShow);

	BOOL	registerWindowCB(HINSTANCE hInstance);
	BOOL	createWindow(HWND hParent, HINSTANCE hInstance, int nCmdShow, LPRECT dRect);
	int		messagePump();
	LRESULT	appWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int		openConfigBox(HWND hParent, HINSTANCE inst);

protected:
	Application app;
	_Window window;

	// Abstract base class function declarations
	virtual void idleProc() =0;
	virtual void shapeWindow() =0;
	virtual void initSaver() =0;
	virtual void cleanUp() =0;
	virtual BOOL saverConfigureDialog(HWND hDlg, UINT msg, WPARAM wpm, LPARAM lpm) =0;

	void setBestPixelFormat();

private:
	// static callback w/ core functionality
	static LRESULT CALLBACK _staticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK _staticDialogProc(HWND hDlg, UINT msg, WPARAM wpm, LPARAM lpm);
};

#endif