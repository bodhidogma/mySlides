/**
*/

#include "stdafx.h"
#include "mySlideSaver.h"


SlideSaver::SlideSaver()
{
	x = 0;
}

SlideSaver::~SlideSaver()
{
	x = 0;
}

int SlideSaver::idleProc()
{
	return 0;
}

void SlideSaver::shapeWindow()
{
}

void SlideSaver::initSaver()
{
}

void SlideSaver::cleanUp()
{
}

BOOL SlideSaver::saverConfigureDialog(HWND hDlg, UINT msg, WPARAM wpm, LPARAM lpm)
{
   switch(msg){
//    case WM_INITDIALOG:
//        InitCommonControls();
//        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wpm)){
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
