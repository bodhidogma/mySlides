/**
*/

#ifndef _MYSLIDESAVER_H_
#define _MYSLIDESAVER_H_

#include "main_window.h"

class SlideSaver : public AppWindow
{
public:
	SlideSaver();
	~SlideSaver();

protected:
	int x;

	// abstract function implementations
	int  idleProc();
	void shapeWindow();
	void initSaver();
	void cleanUp();
	BOOL saverConfigureDialog(HWND hDlg, UINT msg, WPARAM wpm, LPARAM lpm);

};

#endif