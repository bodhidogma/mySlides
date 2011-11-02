// win-slides.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "mySlides.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int PaintImage(HWND hWnd);
FIBITMAP *ReadImage( string &imageName );

/**
*/
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MYSLIDES, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYSLIDES));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
/**
*/
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYSLIDES));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MYSLIDES);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
/**
*/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
/**
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
//			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		PaintImage( hWnd );
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
/**
*/
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

/**
*/
int PaintImage(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT r;

	GetClientRect( hWnd, &r );

	hdc = BeginPaint(hWnd, &ps);
//	HBRUSH br = GetSysColorBrush( COLOR_BACKGROUND );
//	FillRect( hdc, &r, br );

	std::string imageName = "images\\IMG_0138.JPG";
	FIBITMAP *pImage = ReadImage( imageName  );

	if (pImage)
	{
		int winWidth = r.right;
		int winHeight = r.bottom;

		long imgWidth = FreeImage_GetWidth( pImage );
		long imgHeight = FreeImage_GetHeight( pImage );

        //
        // Now calculate stretch factor
        //
        float x_stretch = (float) winWidth / imgWidth;
        float y_stretch = (float) winHeight / imgHeight;
        float stretch;
        if ( x_stretch < 1 || y_stretch < 1 )
            stretch = x_stretch < y_stretch ? x_stretch : y_stretch;
        else
            stretch = x_stretch < y_stretch ? x_stretch : y_stretch;
        SetStretchBltMode( hdc, COLORONCOLOR );
        StretchDIBits( hdc, 
                       ( winWidth - imgWidth * stretch ) / 2,   //XDest
                       ( winHeight - imgHeight * stretch ) / 2, //YDest
                       imgWidth * stretch + .5,              //DestHeight
                       imgHeight * stretch + .5,             //DestWidth
                       0, 0,                                            //XSrc, YSrc
                       imgWidth, imgHeight,       //SrcHeight, SrcWidth
                       FreeImage_GetBits(pImage),
                       FreeImage_GetInfo(pImage),
                       DIB_RGB_COLORS,
                       SRCCOPY );

	}
	EndPaint(hWnd, &ps);
	return 0;
}


/**
*/
#if 0
void LoadFileNames( string base, vector<string> &collection )
{
    if ( base.end()[ -1 ] != '/' )
        base += '/';
    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFile( ( base + "*.jpg" ).c_str(), &fd );
    if ( h != INVALID_HANDLE_VALUE ) {
        bool done = false;
        while ( !done ) {
            if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
                collection.push_back( base + fd.cFileName );
            done = !FindNextFile( h, &fd );
        }
        FindClose( h );
    }
    h = FindFirstFile( ( base + "*" ).c_str(), &fd );
    if ( h != INVALID_HANDLE_VALUE ) {
        bool done = false;
        while ( !done ) {
            const string name = fd.cFileName;
            if ( name != "." && name != ".." ) 
            {
                if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    LoadFileNames( base + fd.cFileName, collection );
            }
            done = !FindNextFile( h, &fd );
        }
        FindClose( h );
    }
}
#endif

/**
*/
FIBITMAP *ReadImage( string &imageName )
{
    cout << "Loading image file " << imageName << "\n";

	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(imageName.c_str(),0);
	FIBITMAP *p = FreeImage_Load(fifmt, imageName.c_str(),0);

	if (p)
	{
		FITAG *tag = NULL;
		FreeImage_GetMetadata(FIMD_EXIF_MAIN, p, "Orientation", &tag);
		short *rot = 0;
		if (tag != NULL) {
			rot = (short*)FreeImage_GetTagValue(tag);
			cout << "\tOrientation: " << *rot << "\n";
		}
		// http://sylvana.net/jpegcrop/exif_orientation.html

		switch (*rot) {
		case 3:		// CCW-180
			p = FreeImage_RotateClassic(p, 180);
			break;
		case 6:		// CCW-270
			p = FreeImage_RotateClassic(p, 270);
			break;
		case 8:		// CCW-90
			p = FreeImage_RotateClassic(p, 90);
			break;
		}
	}
	return p;
}