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


GL_Window*	g_window;
Keys*		g_keys;

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
	glColor4f(1.0f,1.0f,1.0f, 0.5f);
	glBlendFunc(GL_ONE,GL_SRC_ALPHA);					// Set Blending Mode (Cheap / Quick)
	glEnable(GL_BLEND);									// Enable Blending

	// init timer
	SetTimer( window->hWnd, 101, TIMER_DURATION *1000, 0 );

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
void Update (GL_Window* window, DWORD milliseconds)
{
	// process keypress
	if (g_keys->keyDown [VK_ESCAPE] == TRUE)
		TerminateApplication (g_window);

	if (g_keys->keyDown [VK_F1] == TRUE)
		ToggleFullscreen (g_window);

	// other update functions
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
	if (g_slideFactory)
		g_slideFactory->nextSlide();
}