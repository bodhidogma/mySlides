/**
*/

#ifndef _IMAGEFACTORY_H_
#define _IMAGEFACTORY_H_

#include "Image.h"

class ImageFactory
{
public:
	ImageFactory(TCHAR *basePath, int maxWidth, int maxHeight, int winWidth, int winHeight, int limit);
	~ImageFactory();

	void updateWinSize(int width, int height);

	void nextSlide(int doFadeOut);
	int  drawSlide(float FPS);
	float elapsedCheck( float sElapse, int nextSeconds );

private:
	int loadSlides(tstring basePath, int limit);

	tstring searchBase;
	vector<tstring> slideNames;
	int currentName;

	Image *theSlide;
	Image *oldSlide;

	int winWidth, winHeight;
	int maxWidth, maxHeight;
	float sElapsed;
};

#endif