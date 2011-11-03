// win-slides.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "mySlides.h"

#define MAX_LOADSTRING 100

struct SlideData;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void LoadFileNames( tstring base, vector<tstring> &collection );
int PaintImageGDI(HWND hWnd, SlideData *slide);
FIBITMAP *LoadImage( SlideData *slide, long maxWidth, long maxHeight );

// ----
struct SlideData
{
	SlideData (TCHAR *base) {
		searchBase = base;
		current_picture = 0;
		pImage = 0;
	}
	tstring searchBase;
	int current_picture;
	vector<tstring> picture_names;
	FIBITMAP *pImage;
	int width, height;
};

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
	HWND hWnd = InitInstance (hInstance, nCmdShow);
	if (!hWnd)
	{
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYSLIDES));

	// set initial path for images, save data to USERDATA block and kick off timer
	SlideData data( _T("images") );
	SetWindowLong( hWnd, GWL_USERDATA, (long)&data );
	SendMessage( hWnd, WM_TIMER, 0,0 );
	SetTimer( hWnd, 101, 3000, 0 );

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

//	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYSLIDES));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
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
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, 320, 240, NULL, NULL, hInstance, NULL);
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (hWnd)
   {
	   ShowWindow(hWnd, nCmdShow);
	   UpdateWindow(hWnd);
   }
   return hWnd;
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
    static const int maxWidth = GetSystemMetrics( SM_CXSCREEN );
    static const int maxHeight = GetSystemMetrics( SM_CYSCREEN );

    SlideData *slide = (SlideData *) GetWindowLong( hWnd, GWL_USERDATA );

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
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
		if (slide) 
		{
			PaintImageGDI( hWnd, slide );
		}
		break;
	case WM_TIMER:
		if (slide->current_picture == slide->picture_names.size() )
		{
			slide->picture_names.resize(0);
			LoadFileNames( slide->searchBase, slide->picture_names );
			random_shuffle( slide->picture_names.begin(), slide->picture_names.end() );
			slide->current_picture = 0;
		}
		if (slide->pImage )
		{
			FreeImage_Unload( slide->pImage );
			//delete slide->pImage;
		}
		slide->pImage = LoadImage(slide, maxWidth,maxHeight);
		InvalidateRect(hWnd, 0, FALSE);
		slide->current_picture++;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_QUIT:
		if (slide->pImage )
		{
			FreeImage_Unload( slide->pImage );
			//delete slide->pImage;
			slide->pImage = 0;
		}
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
int PaintImageGDI(HWND hWnd, SlideData *slide)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT r;

	GetClientRect( hWnd, &r );

	hdc = BeginPaint(hWnd, &ps);
	HBRUSH br = GetSysColorBrush( COLOR_BACKGROUND );
	FillRect( hdc, &r, br );

	if (slide && slide->pImage)
	{
		int winWidth = r.right;
		int winHeight = r.bottom;

        //
        // Now calculate stretch factor
        //
		float x_stretch = (float) winWidth / slide->width;
		float y_stretch = (float) winHeight / slide->height;
        float stretch;
        if ( x_stretch < 1 || y_stretch < 1 )
            stretch = x_stretch < y_stretch ? x_stretch : y_stretch;
        else
            stretch = x_stretch < y_stretch ? x_stretch : y_stretch;
        SetStretchBltMode( hdc, COLORONCOLOR );
        StretchDIBits( hdc, 
			(int)( winWidth - slide->width * stretch ) / 2,   //XDest
			(int)( winHeight - slide->height * stretch ) / 2, //YDest
			(int)((slide->width * stretch) + .5),				//DestHeight
			(int)((slide->height * stretch) + .5),				//DestWidth
			0, 0,                                       //XSrc, YSrc
			slide->width, slide->height,       //SrcHeight, SrcWidth
			FreeImage_GetBits(slide->pImage),
			FreeImage_GetInfo(slide->pImage),
			DIB_RGB_COLORS,
			SRCCOPY );
	}

	LOGFONT f = {0};
	f.lfHeight = 24;
	//f.lfItalic = TRUE;
	strcpy( f.lfFaceName, _T("Comic Sans MS"));
	HFONT font = CreateFontIndirect(&f);
	SelectObject(hdc, font);

//	SetBkColor(hdc, COLOR_BACKGROUND);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0xFF,0xFF,0));
	tstring slide_name = slide->picture_names[slide->current_picture-1];
	DrawText(hdc, slide_name.c_str(), -1,&r, DT_CENTER);

	EndPaint(hWnd, &ps);
	return 0;
}


/**
*/
void LoadFileNames( tstring base, vector<tstring> &collection )
{
    if ( base.end()[ -1 ] != '/' )
        base += '/';
    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFile( ( base + _T("*.jpg") ).c_str(), &fd );
    if ( h != INVALID_HANDLE_VALUE ) {
        bool done = false;
        while ( !done ) {
            if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
                collection.push_back( base + fd.cFileName );
            done = !FindNextFile( h, &fd );
        }
        FindClose( h );
    }
    h = FindFirstFile( ( base + _T("*") ).c_str(), &fd );
    if ( h != INVALID_HANDLE_VALUE ) {
        bool done = false;
        while ( !done ) {
            const tstring name = fd.cFileName;
            if ( name != _T(".") && name != _T("..") ) 
            {
                if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    LoadFileNames( base + fd.cFileName, collection );
            }
            done = !FindNextFile( h, &fd );
        }
        FindClose( h );
	}
}

/**
*/
FIBITMAP *LoadImage( SlideData *slide, long maxWidth, long maxHeight )
{
	tstring imageName = slide->picture_names[ slide->current_picture ];
	FIBITMAP *p = 0;
	FREE_IMAGE_FORMAT fifmt = _tFreeImage_GetFileType(imageName.c_str(),0);
	switch (fifmt)
	{
#ifdef JPEG_EXIFROTATE
	case FIF_JPEG:
		p = _tFreeImage_Load(fifmt, imageName.c_str(),JPEG_EXIFROTATE);
		break;
#endif
	default:
		p = _tFreeImage_Load(fifmt, imageName.c_str(),0);
	}
	
	if (p)
	{
		long imgWidth =  FreeImage_GetWidth(p);
		long imgHeight = FreeImage_GetHeight(p);

		float x_stretch = (float) maxWidth / imgWidth;
        float y_stretch = (float) maxHeight / imgHeight;
        float stretch;
        if ( x_stretch < 1 || y_stretch < 1 )
            stretch = x_stretch < y_stretch ? x_stretch : y_stretch;
        else
            stretch = x_stretch < y_stretch ? x_stretch : y_stretch;
		
		imgWidth = (long)((imgWidth * stretch) +.5);
		imgHeight = (long)((imgHeight * stretch) +.5);

		p = FreeImage_Rescale(p, imgWidth,imgHeight, FILTER_BOX);
		slide->width = imgWidth;
		slide->height = imgHeight;

#ifndef JPEG_EXIFROTATE
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
#endif
	}
	return p;
}