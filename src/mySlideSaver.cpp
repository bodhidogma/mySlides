/**
*/

#include "stdafx.h"
#include "mySlideSaver.h"

//#define DRAW_BOX

#define	DEF_PREVIEW_DURATION	5	// 5s
#define	DEF_SLIDE_DURATION		10	// 10s
#define DEF_TRANS_DURATION		2	// 2s
#define REGISTRY_PATH			_T("Software\\QuedaNet\\mySlides")

#define MAX_SLIDE_FPS			30
#define MIN_SLIDE_FPS			5

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
	// some 
	state.registryPath = REGISTRY_PATH;

	sql = new SQLite();
	db_init();

//	state.slidePaths.resize(0);
//	state.slidePaths.clear();
}

/**
*/
SlideSaver::~SlideSaver()
{
	if (slideFactory)
		delete slideFactory;
	if (sql)
		sql->close();
}

/**
*/
void SlideSaver::db_init()
{
	if (sql->open("myslides.db") == SQLITE_OK) {
		sql->exec("create table if not exists images("
			"id integer primary key"
			",filepath text"
			",filedate datetime"
			",active boolean"
			")");
//		if (sql->error()) { sql->error_str(); }

		sql->exec("create index if not exists active on images(active)");
//		if (sql->error()) { sql->error_str(); }
	}
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
		state.totalTime = slideFactory->elapsedCheck(state.frameTime,
			state.slideDuration, (float)state.transitionDuration);
		slideFactory->drawSlide(getFPSLimit());
	}

	wglSwapLayerBuffers(window.hDC, WGL_SWAP_MAIN_PLANE);
}

/**
*/
void SlideSaver::idleProc()
{
	if (state.drawOK)
	{
		// update timer
		state.frameTime = state.timer.tick();
		state.totalTime += state.frameTime;
#ifndef DRAW_BOX
		draw();
		if (state.totalTime <= (float)state.transitionDuration+0.5f)
			setFPSLimit(MAX_SLIDE_FPS);
		else
			setFPSLimit(MIN_SLIDE_FPS);
#else
		drawBox();
#endif
	}
}

/**
*/
void SlideSaver::initSaver()
{
	readRegistry();
//	writeRegistry();

	// if in preview mode, do some things a bit different
	if (app.isPreview) {
		state.slideDuration = DEF_PREVIEW_DURATION;
	}

	this->setBestPixelFormat();
	this->window.hRC = wglCreateContext(this->window.hDC);
	wglMakeCurrent(this->window.hDC, this->window.hRC);

	shapeWindow();
	state.drawOK = 1;

#ifndef DRAWBOX
	int limit = 0;
	if (app.isPreview)
		limit = 5;

	slideFactory = new ImageFactory(&state.slidePaths[0],
		window.init.screenW, window.init.screenH, window.init.width, window.init.height, limit);

	setFPSLimit(MAX_SLIDE_FPS);
	state.timer.tick();	// reset timer
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

#if 0
DWORD type, size;
vector<wstring> target;
if ( RegQueryValueExW(
    your_key, // HKEY
    TEXT("ValueName"),
    NULL,
    &type,
    NULL,
    &size ) != ERROR_SUCCESS )
  return;
if ( type == REG_MULTI_SZ )
{
  vector<wchar_t> temp(size/sizeof(wchar_t));

  if ( RegQueryValueExW(
      your_key, // HKEY
      TEXT("ValueName"),
      NULL,
      NULL,
      reinterpret_cast<LPBYTE>(&temp[0]),
      &size ) != ERROR_SUCCESS )
  return;

  size_t index = 0;
  size_t len = wcslen( &temp[0] );
  while ( len > 0 )
  {
    target.push_back(&temp[index]);
    index += len + 1;
    len = wcslen( &temp[index] );
  }
}
#endif

/**
*/
void SlideSaver::setDefaults()
{
	state.slideDuration = DEF_SLIDE_DURATION;
	state.transitionDuration = DEF_TRANS_DURATION;
//	state.slidePaths.push_back( IMAGE_PATH );
}

/**
*/
void SlideSaver::readRegistry()
{
	LONG result;
	HKEY skey;
	DWORD valtype, valsize, val;
	TCHAR cval[256] = {0};

	setDefaults();

	result = RegOpenKeyEx(HKEY_CURRENT_USER, state.registryPath, 0, KEY_READ, &skey);
	if(result != ERROR_SUCCESS)
		return;

	valsize=sizeof(val);
	result = RegQueryValueExA(skey, "slide_dur", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		state.slideDuration = val;
	result = RegQueryValueExA(skey, "trans_dur", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		state.transitionDuration = val;

	valsize = 256*sizeof(TCHAR);
	result = RegQueryValueEx(skey, _T("slide_path"), 0, &valtype, (LPBYTE)&cval, &valsize);
	if(result == ERROR_SUCCESS){
		while (state.slidePaths.size()) state.slidePaths.pop_back();
		state.slidePaths.push_back( cval );
	}
#if 0
	result = RegQueryValueEx(skey, "Particles", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dParticles = val;
#endif
	RegCloseKey(skey);
}

/**
*/
void SlideSaver::writeRegistry()
{
	LONG result;
	HKEY skey;
	DWORD val, disp;

	result = RegCreateKeyEx(HKEY_CURRENT_USER, state.registryPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &skey, &disp);
	if(result != ERROR_SUCCESS)
		return;
	val = state.slideDuration;
	RegSetValueExA(skey, "slide_dur", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = state.transitionDuration;
	RegSetValueExA(skey, "trans_dur", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));

	RegSetValueEx(skey, _T("slide_path"), 0, REG_SZ, (CONST BYTE*)state.slidePaths[0].c_str(),
		state.slidePaths[0].length()*sizeof(TCHAR));
#if 0
	val = dParticles;
	RegSetValueEx(skey, "Particles", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dSize;
#endif
	RegCloseKey(skey);
}

/**
*/
void SlideSaver::initControls(HWND hDlg)
{
	CHAR cval[32];

	SendDlgItemMessage(hDlg, IDC_DISPTIME, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(2), DWORD(120))));
	SendDlgItemMessage(hDlg, IDC_DISPTIME, TBM_SETPOS, 1, LPARAM(state.slideDuration));
	SendDlgItemMessage(hDlg, IDC_DISPTIME, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hDlg, IDC_DISPTIME, TBM_SETPAGESIZE, 0, LPARAM(2));
	SendDlgItemMessage(hDlg, IDC_DISPTIME, TBM_SETTICFREQ, 10, 0);
	_snprintf(cval, 32, "%d Seconds", state.slideDuration);
	SendDlgItemMessageA(hDlg, IDC_DISPTIME_TEXT, WM_SETTEXT, 0, LPARAM(cval));

	SendDlgItemMessage(hDlg, IDC_DIR_LIST, LB_ADDSTRING, 0, LPARAM(state.slidePaths[0].c_str()));
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

/**
*/
BOOL SlideSaver::saverConfigureDialog(HWND hDlg, UINT msg, WPARAM wpm, LPARAM lpm)
{
	int ival;
	CHAR cval[256] = {0};

	switch(msg) {
	case WM_INITDIALOG:
		readRegistry();
		initControls(hDlg);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wpm)) {
		case IDOK:
			state.slideDuration = SendDlgItemMessage(hDlg, IDC_DISPTIME, TBM_GETPOS, 0, 0);
#if 0
			SendDlgItemMessage(hDlg, IDC_DIR_LIST, LB_GETTEXT, 0, LPARAM(cval));

			dCyclones = SendDlgItemMessage(hdlg, CYCLONES, UDM_GETPOS, 0, 0);
			dFrameRateLimit = SendDlgItemMessage(hdlg, FRAMERATELIMIT, TBM_GETPOS, 0, 0);
#endif
			writeRegistry();
			// Fall through
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wpm));
			break;
		case IDC_DEFAULTS:
			setDefaults();
			initControls(hDlg);
			break;
//		case IDC_ABOUT:
		}
		return TRUE;
	case WM_HSCROLL:
		if (HWND(lpm) == GetDlgItem(hDlg, IDC_DISPTIME)) {
			ival = SendDlgItemMessage(hDlg, IDC_DISPTIME, TBM_GETPOS, 0, 0);
//			ival = (ival / 5) * 5;
			_snprintf(cval, 32, "%d Seconds", ival);
			SendDlgItemMessageA(hDlg, IDC_DISPTIME_TEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		return TRUE;
	}
	return FALSE;
}
