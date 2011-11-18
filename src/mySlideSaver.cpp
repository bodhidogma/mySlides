/**
*/

#include "stdafx.h"
#include "mySlideSaver.h"


SlideSaver::SlideSaver()
{
	memset(&state,0,sizeof(_State));
	state.aspectRatio = 1.0;
}

SlideSaver::~SlideSaver()
{
}

void SlideSaver::draw()
{
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();
//	glTranslatef(-10.0f, -10.0f, 0.0f);
	glRotatef(-state.r, 0.0f, 0.0f, 1.0f);

#if 0
    glBegin(GL_QUADS);
	glColor3f(1.0f,0.0f,0.0f); glVertex3f(-10.0f,-10.0f, 0.0f);
	glColor3f(0.0f,1.0f,0.0f); glVertex3f( 10.0f,-10.0f, 0.0f);
	glColor3f(0.0f,0.0f,1.0f); glVertex3f( 10.0f, 10.0f, 0.0f);
	glColor3f(1.0f,1.0f,1.0f); glVertex3f(-10.0f, 10.0f, 0.0f);
    glEnd();
#else
	glBegin(GL_TRIANGLES);
	{
		glColor3f(1.0f,0.0f,0.0f); glVertex3f(-1.0,-1.0f, 0.0f);
		glColor3f(0.0f,1.0f,0.0f); glVertex3f( 1.0,-1.0f, 0.0f);
		glColor3f(0.0f,0.0f,1.0f); glVertex3f(-1.0, 1.0f, 0.0f);
		glColor3f(1.0f,0.0f,0.0f); glVertex3f( 1.0, 1.0f, 0.0f);
		glColor3f(0.0f,1.0f,0.0f); glVertex3f(-1.0, 1.0f, 0.0f);
		glColor3f(0.0f,0.0f,1.0f); glVertex3f( 1.0,-1.0f, 0.0f);
	}
	glEnd();
#endif
	glPopMatrix();

	state.r += .1f;
	if (state.r>360) state.r = 0;

	wglSwapLayerBuffers(window.hDC, WGL_SWAP_MAIN_PLANE);
}


void SlideSaver::idleProc()
{
	if (state.drawOK)
	{
		draw();
		Sleep(30);
	}
}

void SlideSaver::shapeWindow()
{
	RECT r;
	GetClientRect(window.hWnd, &r);

	glViewport(r.left, r.top, r.right - r.left, r.bottom - r.top);

	// GL initial settings
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 0.0);

//	glClearDepth (1.0f);								// Depth Buffer Setup
//	glDepthFunc (GL_LEQUAL);							// The Type Of Depth Testing (Less Or Equal)
	glDisable(GL_DEPTH_TEST);							// Disable Depth Testing
	glShadeModel(GL_SMOOTH);							// Select Smooth Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Set Perspective Calculations To Most Accurate

	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping

	glEnable(GL_BLEND);									// Enable Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	//	glEnable(GL_LINE_SMOOTH);
//	glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
//	glEnable(GL_POLYGON_SMOOTH);
//	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

	// form view frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	state.aspectRatio = float(r.right) / float(r.bottom);
#if 0
	gluPerspective(45.0f, aspectRatio, 1.0f, 100.0f);
#else
//	glOrtho( -50.0f *aspectRatio, 50.0f *aspectRatio,-50.0f, 50.0f, -1.0f, 1.0f);
	glOrtho( -1.0f *state.aspectRatio, 1.0f *state.aspectRatio,-1.0f, 1.0f, -1.0f, 1.0f);
#endif
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void SlideSaver::initSaver()
{
	this->setBestPixelFormat();
	this->window.hRC = wglCreateContext(this->window.hDC);
	wglMakeCurrent(this->window.hDC, this->window.hRC);

	shapeWindow();
	state.drawOK = 1;
}

void SlideSaver::cleanUp()
{
	state.drawOK = 0;

	// kill device context
	ReleaseDC(this->window.hWnd, this->window.hDC);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(this->window.hRC);
}

BOOL SlideSaver::saverConfigureDialog(HWND hDlg, UINT msg, WPARAM wpm, LPARAM lpm)
{
	switch(msg) {
	case WM_COMMAND:
		switch(LOWORD(wpm)) {
		case IDOK:
		// Fall through
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wpm));
			break;
		}
		return TRUE;
	}
	return FALSE;
}
