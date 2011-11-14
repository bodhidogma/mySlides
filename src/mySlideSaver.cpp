/**
*/

#include "stdafx.h"
#include "rsWin32Saver.h"

#include "mySlides.h"
#include "mySlideShow.h"
#include "ImageFactory.h"


#if 0
#ifndef CDS_FULLSCREEN											// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4										// Compilers. By Defining It This Way,
#endif															// We Can Avoid Errors
#endif

#define TIMER_DURATION	15
#define SLEEP_TIME		30

//#define IMAGE_PATH	_T("images")
#define IMAGE_PATH		_T("c:\\src\\myslides\\images")
//#define IMAGE_PATH	_T("d:\\data\\pictures\\dcim-jpeg")
//#define IMAGE_PATH	_T("z:\\media\\photos\\2011")
//#define IMAGE_PATH	_T("z:\\pmcavoy\\pictures\\myinet\\2010\\October")

// Global variables
LPCTSTR registryPath = _T("Software\\Queda\\mySlides");
HGLRC hglrc;
HDC hdc;

int readyToDraw = 0;
float frameTime = 0.0f;
float totalTime = 0.0f;
float aspectRatio = 1.0f;

float r = 0;
ImageFactory *g_slideFactory = NULL;

void draw1()
{
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();
//	glTranslatef(-10.0f, -10.0f, 0.0f);
	glRotatef(-r, 0.0f, 0.0f, 1.0f);

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

	r += .1f;
	if (r>360) r = 0;

#if 0
	int i, j;

	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(i=0; i<dCyclones; i++){
		cyclones[i]->update();
		for(j=(i * dParticles); j<((i+1) * dParticles); j++)
			particles[j]->update();
	}

	// print text
	static float totalTime = 0.0f;
	totalTime += frameTime;
	static std::string str;
	static int frames = 0;
	++frames;
	if(frames == 20){
		str = "FPS = " + to_string(20.0f / totalTime);
		totalTime = 0.0f;
		frames = 0;
	}
	if(kStatistics){
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0f, 50.0f * aspectRatio, 0.0f, 50.0f, -1.0f, 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(1.0f, 48.0f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.0f);
		textwriter->draw(str);

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}
#endif
    wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
}

void draw()
{
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (g_slideFactory)
	{
		g_slideFactory->elapsedCheck( frameTime, TIMER_DURATION );
		g_slideFactory->drawSlide();
	}
    wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
}

void idleProc()
{
	// update time
	static rsTimer timer;
	frameTime = timer.tick();
	totalTime += frameTime;

	if(readyToDraw && !isSuspended && !checkingPassword)
	{
		draw();
		if (totalTime < 1.5)
			Sleep( SLEEP_TIME );
		else
			Sleep(SLEEP_TIME * 6);

		if (totalTime >= TIMER_DURATION)
			totalTime = 0.0;
	}
}

/** called whenever window size changes
*/
void shapeSaverWindow(HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);

	glViewport(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

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
	aspectRatio = float(rect.right) / float(rect.bottom);
#if 0
	gluPerspective(45.0f, aspectRatio, 1.0f, 100.0f);
#else
//	glOrtho( -50.0f *aspectRatio, 50.0f *aspectRatio,-50.0f, 50.0f, -1.0f, 1.0f);
	glOrtho( -1.0f *aspectRatio, 1.0f *aspectRatio,-1.0f, 1.0f, -1.0f, 1.0f);
#endif
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	if (g_slideFactory)
		g_slideFactory->updateWinSize( rect.right, rect.bottom );
}

void initSaver(HWND hwnd)
{
	// Window initialization
	hdc = GetDC(hwnd);
	setBestPixelFormat(hdc);
	hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);

	shapeSaverWindow(hwnd);

	RECT rect;
	GetClientRect(hwnd, &rect);
	static const int maxWidth = GetSystemMetrics( SM_CXSCREEN );
	static const int maxHeight = GetSystemMetrics( SM_CYSCREEN );

	g_slideFactory = new ImageFactory(IMAGE_PATH, maxWidth, maxHeight,
		rect.right, rect.bottom);

	// Initialize text
//	textwriter = new rsText;
}


void cleanUp(HWND hwnd)
{
	// Free memory
	delete g_slideFactory;

	// Kill device context
	ReleaseDC(hwnd, hdc);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
}


void setDefaults()
{

}


// Initialize all user-defined stuff
void readRegistry()
{
#if 0
	LONG result;
	HKEY skey;
	DWORD valtype, valsize, val;

	setDefaults();

	result = RegOpenKeyEx(HKEY_CURRENT_USER, registryPath, 0, KEY_READ, &skey);
	if(result != ERROR_SUCCESS)
		return;

	valsize=sizeof(val);

	result = RegQueryValueEx(skey, "Cyclones", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dCyclones = val;
	result = RegQueryValueEx(skey, "Particles", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dParticles = val;
	result = RegQueryValueEx(skey, "Size", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dSize = val;
	result = RegQueryValueEx(skey, "Complexity", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dComplexity = val;
	result = RegQueryValueEx(skey, "Speed", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dSpeed = val;
	result = RegQueryValueEx(skey, "Stretch", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dStretch = val;
	result = RegQueryValueEx(skey, "ShowCurves", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dShowCurves = val;
	result = RegQueryValueEx(skey, "FrameRateLimit", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dFrameRateLimit = val;

	RegCloseKey(skey);
#endif
}


// Save all user-defined stuff
void writeRegistry()
{
#if 0
    LONG result;
	HKEY skey;
	DWORD val, disp;

	result = RegCreateKeyEx(HKEY_CURRENT_USER, registryPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &skey, &disp);
	if(result != ERROR_SUCCESS)
		return;

	val = dCyclones;
	RegSetValueEx(skey, "Cyclones", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dParticles;
	RegSetValueEx(skey, "Particles", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dSize;
	RegSetValueEx(skey, "Size", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dComplexity;
	RegSetValueEx(skey, "Complexity", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dSpeed;
	RegSetValueEx(skey, "Speed", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dStretch;
	RegSetValueEx(skey, "Stretch", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dShowCurves;
	RegSetValueEx(skey, "ShowCurves", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dFrameRateLimit;
	RegSetValueEx(skey, "FrameRateLimit", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));

	RegCloseKey(skey);
#endif
}


BOOL aboutProc(HWND hdlg, UINT msg, WPARAM wpm, LPARAM lpm){
#if 0
	switch(msg){
	case WM_CTLCOLORSTATIC:
		if(HWND(lpm) == GetDlgItem(hdlg, WEBPAGE)){
			SetTextColor(HDC(wpm), RGB(0,0,255));
			SetBkColor(HDC(wpm), COLORREF(GetSysColor(COLOR_3DFACE)));
			return int(GetSysColorBrush(COLOR_3DFACE));
		}
		break;
    case WM_COMMAND:
		switch(LOWORD(wpm)){
		case IDOK:
		case IDCANCEL:
			EndDialog(hdlg, LOWORD(wpm));
			break;
		case WEBPAGE:
			ShellExecute(NULL, "open", "http://www.reallyslick.com", NULL, NULL, SW_SHOWNORMAL);
		}
	}
#endif
	return FALSE;
}


void initControls(HWND hdlg)
{
#if 0
	char cval[16];

	SendDlgItemMessage(hdlg, CYCLONES, UDM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(10), DWORD(1))));
	SendDlgItemMessage(hdlg, CYCLONES, UDM_SETPOS, 0, LPARAM(dCyclones));

	SendDlgItemMessage(hdlg, PARTICLES, UDM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(10000), DWORD(1))));
	SendDlgItemMessage(hdlg, PARTICLES, UDM_SETPOS, 0, LPARAM(dParticles));

	SendDlgItemMessage(hdlg, PARTICLESIZE, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(1), DWORD(100))));
	SendDlgItemMessage(hdlg, PARTICLESIZE, TBM_SETPOS, 1, LPARAM(dSize));
	SendDlgItemMessage(hdlg, PARTICLESIZE, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, PARTICLESIZE, TBM_SETPAGESIZE, 0, LPARAM(5));
	sprintf(cval, "%d", dSize);
	SendDlgItemMessage(hdlg, SIZETEXT, WM_SETTEXT, 0, LPARAM(cval));

	SendDlgItemMessage(hdlg, COMPLEXITY, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(1), DWORD(10))));
	SendDlgItemMessage(hdlg, COMPLEXITY, TBM_SETPOS, 1, LPARAM(dComplexity));
	SendDlgItemMessage(hdlg, COMPLEXITY, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, COMPLEXITY, TBM_SETPAGESIZE, 0, LPARAM(2));
	sprintf(cval, "%d", dComplexity);
	SendDlgItemMessage(hdlg, COMPLEXITYTEXT, WM_SETTEXT, 0, LPARAM(cval));

	SendDlgItemMessage(hdlg, SPEED, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(1), DWORD(100))));
	SendDlgItemMessage(hdlg, SPEED, TBM_SETPOS, 1, LPARAM(dSpeed));
	SendDlgItemMessage(hdlg, SPEED, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, SPEED, TBM_SETPAGESIZE, 0, LPARAM(10));
	sprintf(cval, "%d", dSpeed);
	SendDlgItemMessage(hdlg, SPEEDTEXT, WM_SETTEXT, 0, LPARAM(cval));

	CheckDlgButton(hdlg, STRETCH, dStretch);

	CheckDlgButton(hdlg, SHOWCURVES, dShowCurves);

	initFrameRateLimitSlider(hdlg, FRAMERATELIMIT, FRAMERATELIMITTEXT);
#endif
}


BOOL screenSaverConfigureDialog(HWND hdlg, UINT msg,
										 WPARAM wpm, LPARAM lpm)
{
#if 0
	int ival;
	char cval[16];

    switch(msg){
    case WM_INITDIALOG:
        InitCommonControls();
        readRegistry();
        initControls(hdlg);
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wpm)){
        case IDOK:
            dCyclones = SendDlgItemMessage(hdlg, CYCLONES, UDM_GETPOS, 0, 0);
			dParticles = SendDlgItemMessage(hdlg, PARTICLES, UDM_GETPOS, 0, 0);
			dSize = SendDlgItemMessage(hdlg, PARTICLESIZE, TBM_GETPOS, 0, 0);
			dComplexity = SendDlgItemMessage(hdlg, COMPLEXITY, TBM_GETPOS, 0, 0);
			dSpeed = SendDlgItemMessage(hdlg, SPEED, TBM_GETPOS, 0, 0);
			dStretch = (IsDlgButtonChecked(hdlg, STRETCH) == BST_CHECKED);
			dShowCurves = (IsDlgButtonChecked(hdlg, SHOWCURVES) == BST_CHECKED);
			dFrameRateLimit = SendDlgItemMessage(hdlg, FRAMERATELIMIT, TBM_GETPOS, 0, 0);
			writeRegistry();
            // Fall through
        case IDCANCEL:
            EndDialog(hdlg, LOWORD(wpm));
            break;
		case DEFAULTS:
			setDefaults();
			initControls(hdlg);
			break;
        case ABOUT:
			DialogBox(mainInstance, MAKEINTRESOURCE(DLG_ABOUT), hdlg, DLGPROC(aboutProc));
		}
        return TRUE;
	case WM_HSCROLL:
		if(HWND(lpm) == GetDlgItem(hdlg, PARTICLESIZE)){
			ival = SendDlgItemMessage(hdlg, PARTICLESIZE, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, SIZETEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, COMPLEXITY)){
			ival = SendDlgItemMessage(hdlg, COMPLEXITY, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, COMPLEXITYTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, SPEED)){
			ival = SendDlgItemMessage(hdlg, SPEED, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, SPEEDTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, FRAMERATELIMIT))
			updateFrameRateLimitSlider(hdlg, FRAMERATELIMIT, FRAMERATELIMITTEXT);
		return TRUE;
    }
#endif
    return FALSE;
}


LONG screenSaverProc(HWND hwnd, UINT msg, WPARAM wpm, LPARAM lpm){
	switch(msg){
	case WM_CREATE:
		readRegistry();
		initSaver(hwnd);
		readyToDraw = 1;
		break;
	case WM_DESTROY:
		readyToDraw = 0;
		cleanUp(hwnd);
		break;
	case WM_SIZE:
		switch (wpm)				// Evaluate Size Action
		{
			case SIZE_MINIMIZED:
				readyToDraw = 0;
			return 0;
			default:
				readyToDraw = 1;
				shapeSaverWindow(hwnd);
			return 0;
		}
		break;
	case WM_KEYDOWN:
		readyToDraw = 0;
		PostQuitMessage(0);
		break;
	}
	return defScreenSaverProc(hwnd, msg, wpm, lpm);
}
