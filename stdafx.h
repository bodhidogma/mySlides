// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
//#pragma comment(linker, "/IGNORE:4786")

//#include "targetver.h"

//#undef UNICODE
//#define UNICODE
#ifdef UNICODE
#define tstring wstring
#define _tFreeImage_GetFileType FreeImage_GetFileTypeU
#define _tFreeImage_Load FreeImage_LoadU
#else
#define tstring string
#define _tFreeImage_GetFileType FreeImage_GetFileType
#define _tFreeImage_Load FreeImage_Load
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <scrnsave.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std;

#include <GL/gl.h>
#include <GL/glu.h>

// Additional headers
#include "FreeImage.h"
