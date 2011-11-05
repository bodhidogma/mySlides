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

	tstring imageName = _T("images/img_7383.jpg");
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
	if ( g_slideImage->x < 1.0 )
		g_slideImage->x += .001f;
	else if (g_slideImage->y < 1.0)
		g_slideImage->y += .001f; 
}

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

/** Draw - called to dispaly / render scene
*/
void Draw (GL_Window* window)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();												// reset matrix
    glBindTexture( GL_TEXTURE_2D, g_slideImage->getTextureID() );	// bind texture

//	glTranslatef( 0.0f, 0.0f, -2.4f );
	glTranslatef( g_slideImage->x, g_slideImage->y, -2.4f );
    glInterleavedArrays( GL_T2F_V3F, 0, g_quadVertices );			// draw texture
    glDrawArrays( GL_QUADS, 0, 4 );

#if 0
		glLoadIdentity ();										// Reset The Modelview Matrix
		glBindTexture(GL_TEXTURE_2D, texture[obj[loop].tex]);	// Bind Our Texture
		glTranslatef(obj[loop].x,obj[loop].y,obj[loop].z);		// Position The Object
		glRotatef(45.0f,1.0f,0.0f,0.0f);						// Rotate On The X-Axis
		glRotatef((obj[loop].spinz),0.0f,0.0f,1.0f);			// Spin On The Z-Axis

		glBegin(GL_TRIANGLES);									// Begin Drawing Triangles
			// First Triangle														    _____
			glTexCoord2f(1.0f,1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);				//	(2)|    / (1)
			glTexCoord2f(0.0f,1.0f); glVertex3f(-1.0f, 1.0f, obj[loop].flap);	//	   |  /
			glTexCoord2f(0.0f,0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);				//	(3)|/

			// Second Triangle
			glTexCoord2f(1.0f,1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);				//	       /|(1)
			glTexCoord2f(0.0f,0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);				//	     /  |
			glTexCoord2f(1.0f,0.0f); glVertex3f( 1.0f,-1.0f, obj[loop].flap);	//	(2)/____|(3)

		glEnd();												// Done Drawing Triangles

		obj[loop].y-=obj[loop].yi;								// Move Object Down The Screen
		obj[loop].spinz+=obj[loop].spinzi;						// Increase Z Rotation By spinzi
		obj[loop].flap+=obj[loop].fi;							// Increase flap Value By fi

		if (obj[loop].y<-18.0f)									// Is Object Off The Screen?
		{
			SetObject(loop);									// If So, Reassign New Values
		}

		if ((obj[loop].flap>1.0f) || (obj[loop].flap<-1.0f))	// Time To Change Flap Direction?
		{
			obj[loop].fi=-obj[loop].fi;							// Change Direction By Making fi = -fi
		}
#endif
}
