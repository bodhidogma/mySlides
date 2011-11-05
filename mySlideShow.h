/**
*/

#ifndef _MYSLIDESHOW_H_
#define _MYSLIDESHOW_H_

// Functions
BOOL Initialize (GL_Window* window, Keys* keys);	// All Your Initialization
void Deinitialize (void);							// All Your DeInitialization
void Update (GL_Window* window, DWORD milliseconds);	// Motion Updates
void Draw (GL_Window* window);							// All Your Scene Drawing

void Timer (GL_Window* window);						// Timer callback
#endif