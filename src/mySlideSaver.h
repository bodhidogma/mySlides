/**
*/

#ifndef _MYSLIDESAVER_H_
#define _MYSLIDESAVER_H_

#include "main_window.h"
#include "ImageFactory.h"
#include "rsTimer.h"
#include "cSaverDB.h"

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

struct _State {
	BOOL drawOK;
	float frameTime;
	float totalTime;
	float aspectRatio;
	//
	float r;
	rsTimer timer;
	//
	int slideDuration;
	int transitionDuration;
	vector<tstring> slidePaths;
	//
	LPCTSTR registryPath;
};

class SlideSaver : public AppWindow
{
public:
	SlideSaver();
	~SlideSaver();

//	BOOL saverConfigureDialog(HWND hDlg, UINT msg, WPARAM wpm, LPARAM lpm);

protected:
	_State state;

	// abstract function implementations
	void idleProc();
	void shapeWindow();
	void initSaver();
	void cleanUp();

	void readRegistry();
	void writeRegistry();
	void setDefaults();
	void initControls(HWND hDlg);
	BOOL saverConfigureDialog(HWND hDlg, UINT msg, WPARAM wpm, LPARAM lpm);

private:
//	cSaverDB *sql;

	void draw();
	void drawBox();

	ImageFactory *slideFactory;

	void db_init();
};

#endif