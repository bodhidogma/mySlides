/**
*/

#include "stdafx.h"
#include "mySlides.h"
#include "mySlideShow.h"
#include "ImageFactory.h"


#if 0
#ifndef CDS_FULLSCREEN											// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4										// Compilers. By Defining It This Way,
#endif															// We Can Avoid Errors
#endif

#define TIMER_DURATION	5
#define IMAGE_PATH	_T("images")
//#define IMAGE_PATH	_T("c:\\src\\myslides\\images")
//#define IMAGE_PATH	_T("d:\\data\\pictures\\dcim-jpeg")
//#define IMAGE_PATH	_T("z:\\media\\photos\\2011")
//#define IMAGE_PATH	_T("z:\\pmcavoy\\pictures\\myinet\\2010\\October")


GL_Window*	g_window;
Keys*		g_keys;

GLuint base;					// base display list for font set
GLYPHMETRICSFLOAT gmf[256];


// our image(s)
ImageFactory *g_slideFactory;

/** Initialize - setup prereqs
*/
BOOL Initialize (GL_Window* window, Keys* keys)
{
	g_window	= window;
	g_keys		= keys;

	// Start Of User Initialization
	static const int maxWidth = GetSystemMetrics( SM_CXSCREEN );
	static const int maxHeight = GetSystemMetrics( SM_CYSCREEN );

	g_slideFactory = new ImageFactory(IMAGE_PATH,
		maxWidth,maxHeight, window->init.width,window->init.height);
	
	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth (1.0f);								// Depth Buffer Setup
	glDepthFunc (GL_LEQUAL);							// The Type Of Depth Testing (Less Or Equal)
	glDisable(GL_DEPTH_TEST);							// Disable Depth Testing
	glShadeModel (GL_SMOOTH);							// Select Smooth Shading
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Set Perspective Calculations To Most Accurate

	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
//	glBlendFunc(GL_ONE,GL_SRC_ALPHA);					// Set Blending Mode (Cheap / Quick)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable(GL_BLEND);									// Enable Blending
//	glColor4f(1.0f,1.0f,1.0f, 1.0f);

	// init timer
//	SetTimer( window->hWnd, 101, TIMER_DURATION *1000, 0 );

	BuildFont(window);

	return TRUE;
}

/** Deinitialize before termination
*/
void Deinitialize (void)
{
	if (g_slideFactory)
		delete g_slideFactory;
}

/** called whenever window is resized
*/
void WindowWasResized(int width, int height)
{
	// Ensure factory has been allocated
	if (g_slideFactory)
		g_slideFactory->updateWinSize(width,height);
}

/** Update - called just before every Draw
*/
void Update (GL_Window* window, DWORD msElapsed)
{
	// process keypress
	if (g_keys->keyDown [VK_ESCAPE] == TRUE)
		TerminateApplication (g_window);

	if (g_keys->keyDown [VK_F1] == TRUE)
		ToggleFullscreen (g_window);

	if (g_keys->keyDown [VK_RIGHT] == TRUE)	// 39 : pick next slide
	{
		if (g_slideFactory)
			g_slideFactory->nextSlide(0);
	}

	// other update functions
	if (g_slideFactory)
		g_slideFactory->elapsedCheck(msElapsed, TIMER_DURATION);
}

/** Draw - called to dispaly / render scene
*/
void Draw (GL_Window* window)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	if (g_slideFactory)
		g_slideFactory->drawSlide();
}

/** Timer callback
*/
void Timer (GL_Window* window)
{
//	if (g_slideFactory) g_slideFactory->nextSlide();
}

/* -- */

GLvoid BuildFont(GL_Window* window)								// Build Our Bitmap Font
{
	HFONT	font;
	HFONT	oldfont;

	base = glGenLists(256);

	font = CreateFont(	-12,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_NORMAL,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						_T("Comic Sans MS"));			// Font Name

	oldfont = (HFONT)SelectObject(window->hDC, font);
//	wglUseFontBitmaps(window->hDC, 0, 255, base);
	wglUseFontOutlines( window->hDC, 0, 255, base,
		0.0f,
		0.02f,	// font thickness in Z-dir
		WGL_FONT_POLYGONS,
		gmf);

	SelectObject(window->hDC, oldfont);
	DeleteObject(font);
}

GLvoid KillFont(GLvoid)
{
	glDeleteLists(base, 265);
}

GLvoid glPrint(const TCHAR *fmt, ...)
{
	TCHAR		text[256];
	va_list		ap;

	if (fmt == NULL)
		return;

	va_start(ap, fmt);
#ifdef UNICODE
	_vsntprintf_s(text, 256, 256, fmt, ap);
#else
	_vsntprintf(text, 256, fmt, ap);
#endif
	va_end(ap);

#if 0
	float length = 0.0f
	for (unsigned int loop=0;loop<(_tcslen(text));loop++)
		length+=gmf[text[loop]].gmfCellIncX;
#endif

	glPushAttrib(GL_LIST_BIT);	// Pushes The Display List Bits
	glListBase(base);
#ifdef UNICODE
	glCallLists(_tcslen(text), GL_UNSIGNED_SHORT, text);
#else
	glCallLists(_tcslen(text), GL_UNSIGNED_BYTE, text);
#endif
	glPopAttrib();
}
