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

//HWND g_hWnd = NULL;
HDC  g_hDC  = NULL;
HGLRC g_hRC = NULL;
//GLuint g_textureID = -1;

struct Vertex
{
    // GL_T2F_V3F
    float tu, tv;
    float x, y, z;
};

Vertex g_quadVertices[] =
{
    { 0.0f,0.0f, -1.0f,-1.0f, -0.0f },
    { 1.0f,0.0f,  1.0f,-1.0f, -0.0f },
    { 1.0f,1.0f,  1.0f, 1.0f, -0.0f },
    { 0.0f,1.0f, -1.0f, 1.0f, -0.0f }
};


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void InitGL(HWND hWnd);
void PaintImageGL( HWND hWnd, SlideData *slide );
int LoadImageGL( SlideData *slide, long maxWidth, long maxHeight );

void ImageSelect(SlideData *slide);
void LoadFileNames( tstring base, vector<tstring> &collection );
int PaintImageGDI(HWND hWnd, SlideData *slide);
FIBITMAP *LoadImage( SlideData *slide, long maxWidth, long maxHeight );
void setGLView( int fov, int width, int height );

// ----
struct SlideData
{
	SlideData (TCHAR *base) {
		searchBase = base;
		current_picture = 0;
		pImage = 0;
		textureID = -1;
	}
	tstring searchBase;
	int current_picture;
	vector<tstring> picture_names;
	FIBITMAP *pImage;
	int width, height;
	GLuint textureID;
};

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

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYSLIDES));
	wcex.hIconSm		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYSLIDES));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;

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

   hWnd = CreateWindowEx(NULL, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW|WS_VISIBLE,
      0, 0, 320, 240, NULL, NULL, hInstance, NULL);
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (hWnd)
   {
	   ShowWindow(hWnd, nCmdShow);
	   UpdateWindow(hWnd);
   }
   return hWnd;
}

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
//	HACCEL hAccelTable;

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
	InitGL(hWnd);

	// set initial path for images, save data to USERDATA block and kick off timer
	SlideData slide( _T("images") );
	SetWindowLong( hWnd, GWL_USERDATA, (long)&slide );
	
	SetTimer( hWnd, 101, 3000, 0 );
	SendMessage( hWnd, WM_TIMER, 0,0 );

	// Main message loop:
#if 0
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#else
	memset( &msg, 0, sizeof(msg) );
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage( &msg, NULL, 0,0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			PaintImageGL( NULL, &slide );
			Sleep(20);
		}
	}
#endif

	// shutdown
	if ( g_hRC != NULL) {
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( g_hRC );
		g_hRC = NULL;
	}
	if ( g_hDC != NULL ) {
		ReleaseDC( hWnd, g_hDC );
		g_hDC = NULL;
	}

	return (int) msg.wParam;
}



/**
*/
void InitGL(HWND hWnd)
{
	GLuint PixelFormat;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;

	g_hDC = GetDC( hWnd );
	PixelFormat = ChoosePixelFormat( g_hDC, &pfd);
	SetPixelFormat( g_hDC, PixelFormat, &pfd);
	g_hRC = wglCreateContext( g_hDC );
	wglMakeCurrent( g_hDC, g_hRC );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable( GL_TEXTURE_2D );
	setGLView( 90, 640, 480);
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
    static const int maxWidth = GetSystemMetrics( SM_CXSCREEN );
    static const int maxHeight = GetSystemMetrics( SM_CYSCREEN );

    SlideData *slide = (SlideData *) GetWindowLong( hWnd, GWL_USERDATA );

	switch (message)
	{
	case WM_SIZE:
		{
			int nWidth  = LOWORD(lParam); 
			int nHeight = HIWORD(lParam);
			glViewport(0, 0, nWidth, nHeight);

			setGLView( 90, nWidth, nHeight );
		}
		break;
#if 0
	case WM_PAINT:
		if (slide) 
		{
			//PaintImageGDI( hWnd, slide );
		}
		break;
#endif
	case WM_TIMER:
		ImageSelect( slide );
//		InvalidateRect(hWnd, 0, FALSE);
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_QUIT:
		if (slide->pImage )
		{
			FreeImage_Unload( slide->pImage );
			slide->pImage = 0;
		}
		glDeleteTextures(1, &slide->textureID);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void ImageSelect(SlideData *slide)
{
    static const int maxWidth = GetSystemMetrics( SM_CXSCREEN );
    static const int maxHeight = GetSystemMetrics( SM_CYSCREEN );

	if (slide->current_picture == slide->picture_names.size() )
	{
		slide->picture_names.resize(0);
		LoadFileNames( slide->searchBase, slide->picture_names );
		random_shuffle( slide->picture_names.begin(), slide->picture_names.end() );
		slide->current_picture = 0;
	}
	if (slide->pImage )	{
		FreeImage_Unload( slide->pImage );
	}
	//slide->pImage = LoadImage(slide, maxWidth,maxHeight);
	LoadImageGL(slide, maxWidth,maxHeight);
//		InvalidateRect(hWnd, 0, FALSE);
//		PaintImageGL( hWnd, slide );

	slide->current_picture++;
}


void PaintImageGL( HWND hWnd, SlideData *slide )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
//	glTranslatef( 0.0f, 0.0f, -1.0f );

    glBindTexture( GL_TEXTURE_2D, slide->textureID );
    glInterleavedArrays( GL_T2F_V3F, 0, g_quadVertices );
    glDrawArrays( GL_QUADS, 0, 4 );

	SwapBuffers( g_hDC );
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
	HBRUSH br = (HBRUSH)GetStockObject(BLACK_BRUSH);
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
	_tcscpy( f.lfFaceName, _T("Comic Sans MS"));
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

void setGLView( int fov, int width, int height )
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// The following code is a fancy bit of math that is eqivilant to calling:
	// gluPerspective( fieldOfView/2.0f, width/height , 0.1f, 255.0f )
	// We do it this way simply to avoid requiring glu.h
	GLfloat zNear = 0.1f;
	GLfloat zFar = 100.0f;
	GLfloat aspect = float(width)/float(height);
	GLfloat fH = (float)tan( float(fov / 360.0f * 3.14159f) ) * zNear;
	GLfloat fW = fH * aspect;
	
	glOrtho(-1*aspect,1*aspect, -1,1,-10,10);
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
int LoadImageGL( SlideData *slide, long maxWidth, long maxHeight )
{
	FIBITMAP *p = 0;

	glDeleteTextures( 1, &slide->textureID );
	if (p = LoadImage( slide, maxWidth, maxHeight ))
	{
		long imgWidth =  FreeImage_GetWidth(p);
		long imgHeight = FreeImage_GetHeight(p);

		p = FreeImage_ConvertTo24Bits(p);

		// This is important to note, FreeImage loads textures in
		// BGR format. Now we could just use the GL_BGR extension
		// But, we will simply swap the B and R components ourselves.
		// Firstly, allocate the new bit data doe the image.
//		BYTE *bits = new BYTE[imgWidth * imgHeight * 3];

		// get a pointer to FreeImage's data.
		BYTE *pixels = (BYTE*)FreeImage_GetBits(p);

		// Iterate through the pixels, copying the data
		// from 'pixels' to 'bits' except in RGB format.
#if 0
		for(unsigned int pix=0; pix<imgWidth * imgHeight; pix++)
		{
			bits[pix*3+0]=pixels[pix*3+2];
			bits[pix*3+1]=pixels[pix*3+1];
			bits[pix*3+3]=pixels[pix*3+0];
		}
#endif
		glGenTextures( 1, &slide->textureID );
		glBindTexture( GL_TEXTURE_2D, slide->textureID );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		// The new 'glTexImage2D' function, the prime difference
		// being that it gets the width, height and pixel information
		// from 'bits', which is the RGB pixel data..
		glTexImage2D( GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0,
				GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels );

		FreeImage_Unload(p);
	}
	return 0;
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
		p = _tFreeImage_Load(fifmt, imageName.c_str(),JPEG_EXIFROTATE|JPEG_ACCURATE);
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