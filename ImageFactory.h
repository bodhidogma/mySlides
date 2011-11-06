/**
*/

#ifndef _IMAGEFACTORY_H_
#define _IMAGEFACTORY_H_

#include "Image.h"

class ImageFactory
{
public:
	ImageFactory(TCHAR *basePath, int maxWidth, int maxHeight, int winWidth, int winHeight);
	~ImageFactory();

	void updateWinSize(int width, int height);

	void nextSlide();
	int drawSlide();

private:
	void loadSlides(tstring basePath);

	tstring searchBase;
	vector<tstring> slideNames;
	int currentName;

	Image *theSlide;

	int winWidth, winHeight;
	int maxWidth, maxHeight;
};

#endif