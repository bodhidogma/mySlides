/**
*/

#include "stdafx.h"
#include "mySlides.h"
#include "mySlideShow.h"
#include "Image.h"

#if 0
#ifndef CDS_FULLSCREEN											// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4										// Compilers. By Defining It This Way,
#endif															// We Can Avoid Errors
#endif 

GL_Window*	g_window;
Keys*		g_keys;

// our image(s)
Image* g_slideImage;

/** Initialize - setup prereqs
*/
BOOL Initialize (GL_Window* window, Keys* keys)
{
	g_window	= window;
	g_keys		= keys;

	// Start Of User Initialization
	g_slideImage = new Image();
	
	static const int maxWidth = GetSystemMetrics( SM_CXSCREEN );
	static const int maxHeight = GetSystemMetrics( SM_CYSCREEN );

	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth (1.0f);								// Depth Buffer Setup
	glDepthFunc (GL_LEQUAL);							// The Type Of Depth Testing (Less Or Equal)
	glDisable(GL_DEPTH_TEST);							// Disable Depth Testing
	glShadeModel (GL_SMOOTH);							// Select Smooth Shading
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Set Perspective Calculations To Most Accurate
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glBlendFunc(GL_ONE,GL_SRC_ALPHA);					// Set Blending Mode (Cheap / Quick)
	glEnable(GL_BLEND);									// Enable Blending

//	tstring imageName = _T("images/img_7383.jpg");
	tstring imageName = _T("images/img_7417.jpg");
//	tstring imageName = _T("images/img_0175.jpg");

	g_slideImage->loadImageTexture( imageName, maxWidth,maxHeight );

	return TRUE;
}

/** Deinitialize before termination
*/
void Deinitialize (void)
{
	if (g_slideImage)
		delete g_slideImage;
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

	RECT r;
	GetClientRect( window->hWnd, &r );

	g_slideImage->Draw(r.right,r.bottom);
}
