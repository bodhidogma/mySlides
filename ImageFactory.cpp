/**
*/

#include "stdafx.h"
#include "ImageFactory.h"

#define IMAGE_EXTENSION _T("*.jpg")
//#define IMAGE_EXTENSION _T("*.cr2")

/**
*/
ImageFactory::ImageFactory(TCHAR *basePath,
	int theMaxWidth, int theMaxHeight, int theWinWidth, int theWinHeight)
{
	searchBase = basePath;
	currentName = 0;
	theSlide = oldSlide = NULL;
	sElapsed = 0;

	// save max screen res for loading textures
	maxWidth = theMaxWidth;
	maxHeight = theMaxHeight;

	winWidth = theWinWidth;
	winHeight = theWinHeight;

	// pick next slide
	this->nextSlide(1);
}

/**
*/
ImageFactory::~ImageFactory()
{
	if (theSlide)
		delete theSlide;
	if (oldSlide)
		delete oldSlide;
}

/**
*/
void ImageFactory::updateWinSize(int width, int height)
{
	winWidth = width;
	winHeight = height;
}

void ImageFactory::elapsedCheck(float sElapse, int nextSeconds)
{
	this->sElapsed += sElapse;

	if (this->sElapsed >= (float)nextSeconds)
	{
		nextSlide(1);
	}
}

/**
*/
void ImageFactory::nextSlide(int doFadeOut)
{
	sElapsed = 0;	// reset elapsed time
	if (currentName == slideNames.size())
	{
		slideNames.resize(0);
		loadSlides(searchBase);
		random_shuffle( slideNames.begin(), slideNames.end() );
		currentName = 0;
	}
	if (oldSlide)
	{
		delete oldSlide;
		oldSlide = NULL;
	}
	if (doFadeOut && theSlide)
	{
		oldSlide = theSlide;
		oldSlide->setOld();
	}

	// load new slide
	if (!slideNames.empty())
		theSlide = new Image( slideNames[ currentName ], maxWidth,maxHeight);

	// iterate to next slide
	currentName++;
}

/**
*/
int ImageFactory::drawSlide()
{
	if (oldSlide && glIsEnabled(GL_BLEND))
		oldSlide->Draw(winWidth, winHeight);

	if (theSlide) {
		theSlide->Draw(winWidth, winHeight);
	}

	return 0;
}

/**
*/
void ImageFactory::loadSlides(tstring basePath)
{
	if ( basePath.end()[ -1 ] != '/' )
		basePath += '/';
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile( ( basePath + IMAGE_EXTENSION ).c_str(), &fd );
	if ( h != INVALID_HANDLE_VALUE ) {
		bool done = false;
		while ( !done ) {
			if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
				slideNames.push_back( basePath + fd.cFileName );
			done = !FindNextFile( h, &fd );
		}
		FindClose( h );
	}
	h = FindFirstFile( ( basePath + _T("*") ).c_str(), &fd );
	if ( h != INVALID_HANDLE_VALUE ) {
		bool done = false;
		while ( !done ) {
			const tstring name = fd.cFileName;
			if ( name != _T(".") && name != _T("..") )
			{
				if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
					loadSlides( basePath + fd.cFileName);
			}
			done = !FindNextFile( h, &fd );
		}
		FindClose( h );
	}
}
