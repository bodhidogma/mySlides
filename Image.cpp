/**
*/

#include "stdafx.h"
#include "mySlides.h"
#include "Image.h"


Image::Image()
{
	memset(&this->m, 0, sizeof(Metadata));
	this->x = this->y = 0.0;
}

Image::~Image()
{
	glDeleteTextures( 1, &this->textureID );
}

int Image::loadImageTexture(tstring imageName, int maxWidth, int maxHeight)
{
	FIBITMAP *dib = 0;
	this->m.width = this->m.height = 0;

	// get image format
	FREE_IMAGE_FORMAT fifmt = _tFreeImage_GetFileType(imageName.c_str(),0);

	// load image with relevant options
	switch (fifmt)
	{
#ifdef JPEG_EXIFROTATE
	case FIF_JPEG:
		dib = _tFreeImage_Load(fifmt, imageName.c_str(),JPEG_EXIFROTATE|JPEG_ACCURATE);
		break;
#endif
	default:
		dib = _tFreeImage_Load(fifmt, imageName.c_str(),0);
	}
	
	// unable to load image, return
	if (!dib)
		return 0;

	// get original image size
	this->m.width  = FreeImage_GetWidth(dib);
	this->m.height = FreeImage_GetHeight(dib);

	// rescale image to size of screen (we don't need a texture bigger than screen size)
	float x_stretch = (float) maxWidth / this->m.width;
    float y_stretch = (float) maxHeight / this->m.height;
    float stretch;
    if ( x_stretch < 1 || y_stretch < 1 )
        stretch = x_stretch < y_stretch ? x_stretch : y_stretch;
    else
        stretch = x_stretch < y_stretch ? x_stretch : y_stretch;

	// new image dimensions
	this->m.width = (unsigned int)((this->m.width * stretch) +.5);
	this->m.height = (unsigned int)((this->m.height * stretch) +.5);

	dib = FreeImage_Rescale(dib, this->m.width,this->m.height, FILTER_BOX);

	// -- save image as texture --

	// FreeImage loads images in BGR format
	dib = FreeImage_ConvertTo24Bits(dib);

	// get a pointer to FreeImage's data.
	BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);

	glGenTextures( 1, &this->textureID );
	glBindTexture( GL_TEXTURE_2D, this->textureID );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexImage2D( GL_TEXTURE_2D, 0, 3, this->m.width, this->m.height, 0,
			GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels );

	FreeImage_Unload(dib);
	dib = 0;

	return 1;
}

int Image::Draw()
{
	return 0;
}