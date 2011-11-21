/**
*/

#ifndef _MYSLIDESAVER_H_
#define _MYSLIDESAVER_H_

#include "main_window.h"
#include "ImageFactory.h"
#include "rsTimer.h"

struct _State {
	BOOL drawOK;
	float frameTime;
	float totalTime;
	float aspectRatio;
	//
	float r;
	rsTimer timer;
	int slideDuration;
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
	void draw();
	void drawBox();

	ImageFactory *slideFactory;
};

#endif