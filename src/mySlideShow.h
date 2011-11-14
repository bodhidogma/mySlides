/**
*/

#ifndef _MYSLIDESHOW_H_
#define _MYSLIDESHOW_H_

// Functions
BOOL Initialize (GL_Window* window, Keys* keys);
void Deinitialize (void);

void WindowWasResized(int width, int height);

void Update (GL_Window* window, DWORD milliseconds);	// Motion Updates
void Draw (GL_Window* window);							// All Your Scene Drawing
void Timer (GL_Window* window);

GLvoid BuildFont(GL_Window* window);
GLvoid KillFont(GLvoid);
GLvoid glPrint(const TCHAR *fmt, ...);

#endif