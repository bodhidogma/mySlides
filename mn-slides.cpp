//
// SlideShow.cpp
//
// by Mark Nelson - 2001
//
// This command line Win32 program uses Intel's JPEG Library
// to repeatedly play a randomized slide show of all JPG files
// in a directory specified on the command line.
//
// This program is built with Visual C++ 6.0, and only needs
// the Intel library to build, compile, and run. See the
// accompanying magazine article for instructions on how to
// build.
//
// Usage: SlideShow directory-name
//
#pragma warning( disable : 4786 )

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std;

#include "FreeImage.h"

//
// Forward declarations
//
HWND CreateOutputWindow();
void LoadFileNames( string base, vector<string> &collection );
FIBITMAP *CreateImage( string &name );

//
// This little structure is used to avoid accessing
// global variables in the WndProc. All of the data
// used in the WndProc is stored in an object of this type,
// and stored in the GWL_USER DWORD associated with the
// window.
//
struct WindowData
{
    WindowData( char *base )
    {
        searchBase = base;
        current_picture = 0;
        pImage = 0;
    }
    string searchBase;
    int current_picture;
    vector<string> picture_names;
    FIBITMAP *pImage;
};

//
// This program starts off life as a console program, expecting
// to see asearch path on the command line. It starts off the 
// process by constructing the window data, constructing the 
// main window of the program, then stashing the pointer to
// the data object in the GWL_USERDATA block. Things are all
// ready to go at that point.
//
// The program has a five second timer, and automatically
// cycles to the next picture each timer tick. The names of
// all the pictures are stored in a vector. When the program
// has reached the last picture in the vector, it rescans the
// directory and starts over.
//
// Pushing the first WM_TIMER event to the message loop causes
// the program to scan the directory for the first time.
//
int main(int argc, char* argv[])
{
    if ( argc < 2 ) 
    {
        cout << "Usage: SlideShow directory\n\n";
        return 1;
    }
    srand( (unsigned) time( NULL ) );
    WindowData data( argv[ 1 ] );
    HWND hWnd = CreateOutputWindow();
    SetWindowLong( hWnd, GWL_USERDATA, (long) &data );
    SendMessage( hWnd, WM_TIMER, 0, 0 );
    ShowWindow( hWnd, SW_SHOW);
    SetTimer( hWnd, 101, 5000, 0 );
    //
    // Everything is ready to go, all we do from this
    // point is pump the message loop.
    //
    MSG msg;
    while ( GetMessage(&msg, NULL, 0, 0 ))
    {  
        ::TranslateMessage(&msg);
	    ::DispatchMessage(&msg);
    }
    return 0;
}

//
// The WndProc is pretty simple, it only has to handle a few messages.
// The only ones of interest are WM_PAINT and WM_TIMER. They are
// documented inline with the code.
//
LRESULT CALLBACK WndProc( HWND hwnd,
                          UINT uMsg,
                          WPARAM wParam,
                          LPARAM lParam )
{
    static const int width = GetSystemMetrics( SM_CXSCREEN );
    static const int height = GetSystemMetrics( SM_CYSCREEN );

    WindowData *w = (WindowData *) GetWindowLong( hwnd, GWL_USERDATA );
    switch ( uMsg )
    {
    //
    // When WM_PAINT is called, we should already have the data set up
    // in the w->pImage object. All we have to do here is paint the
    // background, then BLT the bitmap on to the screen. The scaling
    // calculation is there to insure that I use up as much space as 
    // possible without clipping or distorting.
    //
    case WM_PAINT :
        {
            RECT r;
            GetClientRect( hwnd, &r );
            PAINTSTRUCT ps;
            HDC dc = BeginPaint( hwnd, &ps );
            HBRUSH br = GetSysColorBrush( COLOR_BACKGROUND );
            FillRect( dc, &r, br );

			if ( w->pImage)
			{
				long imgWidth, imgHeight;

				imgWidth = FreeImage_GetWidth( w->pImage );
				imgHeight = FreeImage_GetHeight( w->pImage );

                //
                // Now calculate stretch factor
                //
                float x_stretch = (float) width / imgWidth;
                float y_stretch = (float) height / imgHeight;
                float stretch;
                if ( x_stretch < 1 || y_stretch < 1 )
                    stretch = x_stretch < y_stretch ? x_stretch : y_stretch;
                else
                    stretch = x_stretch < y_stretch ? x_stretch : y_stretch;
                SetStretchBltMode( dc, COLORONCOLOR );
                StretchDIBits( dc, 
                               ( width - imgWidth * stretch ) / 2,   //XDest
                               ( height - imgHeight * stretch ) / 2, //YDest
                               imgWidth * stretch + .5,              //DestHeight
                               imgHeight * stretch + .5,             //DestWidth
                               0, 0,                                            //XSrc, YSrc
                               imgWidth, imgHeight,       //SrcHeight, SrcWidth
                               FreeImage_GetBits(w->pImage),
                               FreeImage_GetInfo(w->pImage),
                               DIB_RGB_COLORS,
                               SRCCOPY );

			}
            EndPaint( hwnd, &ps );
            return 0L;
        }
    //
    // This isn't your everyday window - I have to do a little bit of fiddling
    // to get it to take over the entire screen. No caption, minimize button, or
    // any of that, I want the whole screen for picture display.
    //
    case WM_CREATE :
        {
            RECT work_area;
            SystemParametersInfo( SPI_GETWORKAREA, 0, &work_area, 0 );
            DWORD dwStyle = GetWindowLong( hwnd, GWL_STYLE);        
            dwStyle &= ~( WS_CAPTION );
            SetWindowLong( hwnd, GWL_STYLE, dwStyle );          
            SetWindowPos( hwnd, NULL, 0, 0, width, height, SWP_DRAWFRAME | SWP_NOZORDER ); 
            return 0;
        };
    //
    // If the w->current_picture index shows that I'm all the way 
    // through the list, I go back and rescan the directory structure
    // containing the pictures. (Note that when the program first 
    // starts this will be true, because the vector.size() will be
    // 0). After that, I just delete the old picture, then call the
    // CreateImage() function to load the next one. Once that's done
    // I invalidate the screen to force a paint event.
    //
    case WM_TIMER :
        {
            if ( w->current_picture == w->picture_names.size() )
            {
                w->picture_names.resize( 0 );
                LoadFileNames( w->searchBase, w->picture_names );
                cout << "Found " 
                     << w->picture_names.size() 
                     << " pictures at " 
                     << w->searchBase 
                     << "\n";
                random_shuffle( w->picture_names.begin(), w->picture_names.end() );
                w->current_picture = 0;
            }
            if ( w->pImage )
            {
				FreeImage_Unload( w->pImage );
/*
                ijlFree( w->pImage );
                delete[] w->pImage->DIBBytes;
                delete w->pImage;
*/
            }
            w->pImage = CreateImage( w->picture_names[ w->current_picture ] );
            InvalidateRect( hwnd, 0, FALSE );
            w->current_picture++;
        }
        return 0;
    case WM_CHAR :
    case WM_LBUTTONDOWN : 
        PostQuitMessage(0);
        break;
    case WM_QUIT :
        if ( w->pImage )
        {
			FreeImage_Unload( w->pImage );
			w->pImage = 0;

/*
            ijlFree( w->pImage );
            delete[] w->pImage->DIBBytes;
            delete w->pImage;
            w->pImage = 0;
*/
        }
        break;
    }
    return ::DefWindowProc( hwnd, uMsg, wParam, lParam );
}

//
// This routine just loads all the JPG files in the directory
// tree into the vector of strings. It is called recursively
// as subdirectories are encountered.
//
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

//
// This routine uses the Intel library to load
// the JPEG image into Intel's image structure.
// The code was lifted more or less intact from
// Intel's demo program. Note that the load is
// a two-step process. Step one gets the JPEG image
// file paramters, step two loads it into a Device
// Independent Bitmap, which makes it nice and easy
// to display.
//
FIBITMAP *CreateImage( string &name )
{
    cout << "Loading image file " << name << "\n";

	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(name.c_str(),0);
	FIBITMAP *p = FreeImage_Load(fifmt, name.c_str(),0);

	FITAG *tag = NULL;
	FIMETADATA *mdhandle = NULL;
	mdhandle = FreeImage_FindFirstMetadata(FIMD_EXIF_MAIN, p, &tag);
	if(mdhandle) {
		do {
		// process the tag
			cout << "\t" << FreeImage_GetTagKey(tag) << ":" << (char *)FreeImage_GetTagValue(tag) << "\n";
		// ...
		} while(FreeImage_FindNextMetadata(mdhandle, &tag));
		FreeImage_FindCloseMetadata(mdhandle);
	}
/*
    FIBITMAP *p = new JPEG_CORE_PROPERTIES;
    try
    {
        if( ijlInit( p ) != IJL_OK )
            throw "Cannot initialize Intel JPEG library\n";
        p->JPGFile = name.c_str();
        if( ijlRead( p, IJL_JFILE_READPARAMS ) != IJL_OK )
            throw "Cannot read JPEG file header\n";
        switch( p->JPGChannels )
        {
            case 1:  p->JPGColor    = IJL_G;
                     p->DIBChannels = 3;
                     p->DIBColor    = IJL_BGR;
                     break;
            case 3:  p->JPGColor    = IJL_YCBCR;
                     p->DIBChannels = 3;
                     p->DIBColor    = IJL_BGR;
                     break;
            case 4:  p->JPGColor    = IJL_YCBCRA_FPX;
                     p->DIBChannels = 4;
                     p->DIBColor    = IJL_RGBA_FPX;
                     break;
            default: p->DIBColor = (IJL_COLOR)IJL_OTHER;
                     p->JPGColor = (IJL_COLOR) IJL_OTHER;
                     p->DIBChannels = p->JPGChannels;
                     break;
        }
        p->DIBWidth    = p->JPGWidth;
        p->DIBHeight   = p->JPGHeight;
        p->DIBPadBytes = IJL_DIB_PAD_BYTES( p->DIBWidth, p->DIBChannels );
        int imageSize = p->DIBWidth * p->DIBChannels + p->DIBPadBytes;
        imageSize *= p->DIBHeight;
        p->DIBBytes = new BYTE[ imageSize ];
        if ( ijlRead( p, IJL_JFILE_READWHOLEIMAGE ) != IJL_OK )
            throw "Cannot read image data\n";
        if ( p->DIBColor == IJL_RGBA_FPX )
            throw "Conversion code needed here. Left out of the demo program\n";
    } 
    catch( const char *s )
    {
        cout << s;
        if ( p->DIBBytes )
            delete[] p->DIBBytes;
        ijlFree( p );
        delete p;
        return 0;
    }
*/
    return p;
}

HWND CreateOutputWindow()
{
    WNDCLASS wc = { 0,
                    WndProc,
                    0,
                    0,
                    (HINSTANCE) ::GetModuleHandle( NULL ),
                    0, 
                    LoadCursor( NULL, IDC_ARROW ), 
                    0,
                    NULL,
                    "JpegWindowClass" };
    RegisterClass( &wc );
    return CreateWindow( "JpegWindowClass",
                         "JpegWindow",
                         0, //WS_VISIBLE,
                         0, 0,
                         1, 1, 
                         0,
                         0,
                         (HINSTANCE) ::GetModuleHandle( NULL ),
                         0 );
}

