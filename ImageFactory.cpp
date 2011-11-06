/**
*/

#include "stdafx.h"
#include "ImageFactory.h"

/**
*/
ImageFactory::ImageFactory(TCHAR *basePath,
	int theMaxWidth, int theMaxHeight, int theWinWidth, int theWinHeight)
{
	searchBase = basePath;
	currentName = 0;
	theSlide = NULL;

	// save max screen res for loading textures
	maxWidth = theMaxWidth;
	maxHeight = theMaxHeight;

	winWidth = theWinWidth;
	winHeight = theWinHeight;

	// pick next slide
	this->nextSlide();
}

/**
*/
ImageFactory::~ImageFactory()
{
	if (theSlide)
		delete theSlide;
}

/**
*/
void ImageFactory::updateWinSize(int width, int height)
{
	winWidth = width;
	winHeight = height;
}

/**
*/
void ImageFactory::nextSlide()
{
	if (currentName == slideNames.size())
	{
		slideNames.resize(0);
		loadSlides(searchBase);
//		random_shuffle( slideNames.begin(), slideNames.end() );
		currentName = 0;
	}
	if (theSlide)
		delete theSlide;

	// load new slide
	theSlide = new Image( slideNames[ currentName ], maxWidth,maxHeight);

	// iterate to next slide
	currentName++;
}

/**
*/
int ImageFactory::drawSlide()
{
	if (theSlide)
		theSlide->Draw(winWidth, winHeight);

	return 0;
}

/**
*/
void ImageFactory::loadSlides(tstring basePath)
{
	if ( basePath.end()[ -1 ] != '/' )
		basePath += '/';
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile( ( basePath + _T("*.jpg") ).c_str(), &fd );
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
