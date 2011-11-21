// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma warning(disable: 4786)
#pragma comment(linker, "/IGNORE:4786")

//#include "targetver.h"

//#undef UNICODE
//#define UNICODE

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <mmsystem.h>
#include <CommCtrl.h>
#include <Shellapi.h>

#pragma comment( lib, "Winmm.lib" )
#pragma comment( lib, "ComCtl32.lib" )

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// CPP RunTime Header Files
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std;

// OpenGL 
#include <GL/gl.h>
#include <GL/glu.h>

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )

// Additional headers
#include "FreeImage.h"

// common #defines
#define MAX_LOADSTRING 100

#ifdef UNICODE
#define tstring wstring
#define _tFreeImage_GetFileType FreeImage_GetFileTypeU
#define _tFreeImage_Load FreeImage_LoadU
#else
#define tstring string
#define _tFreeImage_GetFileType FreeImage_GetFileType
#define _tFreeImage_Load FreeImage_Load
#endif

// MSVC6 workarounds (MSC_VER = 1200)
#ifndef SM_XVIRTUALSCREEN
#define SM_XVIRTUALSCREEN 76
#define SM_YVIRTUALSCREEN 77
#define SM_CXVIRTUALSCREEN 78
#define SM_CYVIRTUALSCREEN 79
#endif
#ifndef GWLP_USERDATA
#define GWLP_USERDATA GWL_USERDATA
#endif
#ifndef SetWindowLongPtr
#define SetWindowLongPtr SetWindowLong
#endif
