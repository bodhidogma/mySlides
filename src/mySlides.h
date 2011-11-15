/**
*/

#ifndef _MY_SLIDES_H_
#define _MY_SLIDES_H_

#pragma once

#include "resource.h"

/** ---
*/

typedef struct {									// Structure For Keyboard Stuff
	BOOL keyDown [256];								// Holds TRUE / FALSE For Each Key
} Keys;												// Keys

typedef struct {									// Contains Information Vital To Applications
	HINSTANCE		hInstance;						// Application Instance
	TCHAR			className[100];					// Application ClassName
} Application;										// Application

typedef struct {									// Window Creation Info
	Application*		application;				// Application Structure
	TCHAR				title[100];				// Window Title
	int					width;						// Width
	int					height;						// Height
	int					bitsPerPixel;				// Bits Per Pixel
	BOOL				isFullScreen;				// FullScreen?
} GL_WindowInit;									// GL_WindowInit

typedef struct {									// Contains Information Vital To A Window
	Keys*				keys;						// Key Structure
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context
	GL_WindowInit		init;						// Window Init
	BOOL				isVisible;					// Window Visible?
	DWORD				lastTickCount;				// Tick Counter
} GL_Window;										// GL_Window

/** ---
*/

void TerminateApplication (GL_Window* window);		// Terminate The Application
void ToggleFullscreen (GL_Window* window);			// Toggle Fullscreen / Windowed Mode

#endif