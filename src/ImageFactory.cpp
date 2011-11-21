/**
*/

#include "stdafx.h"
#include "ImageFactory.h"

#define IMAGE_EXTENSION _T("*.jpg")
//#define IMAGE_EXTENSION _T("*.cr2")

/**
*/
ImageFactory::ImageFactory(TCHAR *basePath,
	int theMaxWidth, int theMaxHeight, int theWinWidth, int theWinHeight, int limit)
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

	slideNames.resize(0);
	int cnt = loadSlides(searchBase,limit);
	currentName = slideNames.size();

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

float ImageFactory::elapsedCheck(float sElapse, int nextSeconds)
{
	this->sElapsed += sElapse;

	if (this->sElapsed >= (float)nextSeconds) {
		nextSlide(1);
	}
	return this->sElapsed;
}

/**
*/
void ImageFactory::nextSlide(int doFadeOut)
{
	sElapsed = 0;	// reset elapsed time
	if (currentName == slideNames.size())
	{
		srand( (int)time(NULL) );
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
int ImageFactory::drawSlide(float FPS)
{
	if (oldSlide && glIsEnabled(GL_BLEND)) {
		oldSlide->Draw(winWidth, winHeight, FPS);
	}

	if (theSlide) {
		theSlide->Draw(winWidth, winHeight, FPS);
	}

	return 0;
}

/**
*/
int ImageFactory::loadSlides(tstring basePath, int limit)
{
	int cnt = 0;
	int newLimit = 0;

	if (limit < 0) return 0;

	if ( basePath.end()[ -1 ] != '/' )
		basePath += '/';
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile( ( basePath + IMAGE_EXTENSION ).c_str(), &fd );
	if ( h != INVALID_HANDLE_VALUE ) {
		bool done = false;
		while ( !done ) {
			if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ){
				slideNames.push_back( basePath + fd.cFileName );
				cnt++;
			}
			if (limit && cnt == limit)
				done = TRUE;
			else 
				done = !FindNextFile( h, &fd );
		}
		FindClose( h );
	}
	if (cnt == limit)
		return cnt;

	h = FindFirstFile( ( basePath + _T("*") ).c_str(), &fd );
	if ( h != INVALID_HANDLE_VALUE ) {
		bool done = false;
		while ( !done ) {
			const tstring name = fd.cFileName;
			if ( name != _T(".") && name != _T("..") )
			{
				if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					if (limit) newLimit = limit-cnt;
					cnt += loadSlides( basePath + fd.cFileName, newLimit);
				}
			}
			if (limit && cnt == limit)
				done = TRUE;
			else
				done = !FindNextFile( h, &fd );
		}
		FindClose( h );
	}
	return cnt;
}
