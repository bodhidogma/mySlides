/**
*/

#include "stdafx.h"
#include "mySlideSaver.h"

#define DRAW_BOX
#define TIMER_DURATION	15

//#define IMAGE_PATH	_T("images")
//#define IMAGE_PATH	_T("d:\\data\\pictures\\dcim-jpeg")
//#define IMAGE_PATH	_T("z:\\media\\photos")
#define IMAGE_PATH	_T("z:\\media\\photos\\2011")

//#define IMAGE_PATH	_T("z:\\pmcavoy\\pictures\\myinet\\2010\\October")

/**
*/
SlideSaver::SlideSaver()
{
	memset(&state,0,sizeof(_State));
	state.aspectRatio = 1.0;
	slideFactory = NULL;
}

/**
*/
SlideSaver::~SlideSaver()
{
	if (slideFactory)
		delete slideFactory;
}

/**
*/
void SlideSaver::drawBox()
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
		glColor3f(1.0f,0.0f,0.0f); glVertex3f(-1.0, 0.0f, 0.0f);	// 2\   //
		glColor3f(0.0f,1.0f,0.0f); glVertex3f( 0.0f,-1.0, 0.0f);	// | \  //
		glColor3f(0.0f,0.0f,1.0f); glVertex3f( 0.0f, 1.0, 0.0f);	// 0--1 //

		glColor3f(1.0f,0.0f,0.0f); glVertex3f( 1.0, 0.0f, 0.0f);	// 1--0 //
		glColor3f(0.0f,1.0f,0.0f); glVertex3f( 0.0, 1.0f, 0.0f);	//  \ | //
		glColor3f(0.0f,0.0f,1.0f); glVertex3f( 0.0,-1.0f, 0.0f);	//   \2 //
	}
	glEnd();
#endif
	glPopMatrix();

#define RPM	1
	// looking for 1 rotation / min
	if (window.timeStep == 0.0f)
		setFPSLimit(25);

	state.r += (float)(360.0 / (getFPSLimit()*(60.0/RPM)));
	if (state.r>360) state.r = 0;

	wglSwapLayerBuffers(window.hDC, WGL_SWAP_MAIN_PLANE);
}

/**
*/
void SlideSaver::draw()
{
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (slideFactory) {
		state.totalTime = slideFactory->elapsedCheck(state.frameTime, TIMER_DURATION);
		slideFactory->drawSlide(getFPSLimit());
	}

	wglSwapLayerBuffers(window.hDC, WGL_SWAP_MAIN_PLANE);
}

/**
*/
void SlideSaver::idleProc()
{
	// update timer
	state.frameTime = state.timer.tick();
	state.totalTime += state.frameTime;

	if (state.drawOK)
	{
#ifdef DRAW_BOX
		drawBox();
#else
		draw();

		if (state.totalTime < 4.0f)
			setFPSLimit(30);
		else
			setFPSLimit(4);
#endif
	}
}

/**
*/
void SlideSaver::initSaver()
{
	this->setBestPixelFormat();
	this->window.hRC = wglCreateContext(this->window.hDC);
	wglMakeCurrent(this->window.hDC, this->window.hRC);

	shapeWindow();
	state.drawOK = 1;

#ifndef DRAWBOX
	slideFactory = new ImageFactory(IMAGE_PATH,
		window.init.screenW, window.init.screenH, window.init.width, window.init.height);

	setFPSLimit(30);
#endif
}

/**
*/
void SlideSaver::cleanUp()
{
	state.drawOK = 0;

	// kill device context
	ReleaseDC(this->window.hWnd, this->window.hDC);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(this->window.hRC);
}

/**
*/
void SlideSaver::shapeWindow()
{
	RECT r;
	GetClientRect(window.hWnd, &r);

	glViewport(r.left, r.top, r.right - r.left, r.bottom - r.top);
	window.init.width = r.right - r.left;
	window.init.height = r.bottom - r.top;

	if (slideFactory)
		slideFactory->updateWinSize(window.init.width, window.init.height);

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

/**
*/
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
