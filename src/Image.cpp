/**
*/

#include "stdafx.h"
#include "Image.h"

extern GLvoid glPrint(const TCHAR *fmt, ...);

#define FADE_DURATION	3.0f

/**
*/
Image::Image(tstring imageName, int maxWidth, int maxHeight, float fadeDur)
{
	memset(&this->m, 0, sizeof(Metadata));
	fade_alpha = 0.0;	// start w/ black
	
	x = y = 0.0;
	isNew = TRUE;	// this image is always new (unless modified)
	fade_dur = fadeDur;

	m.name = imageName;
	loadImageTexture(imageName,maxWidth,maxHeight);
}

/**
*/
Image::~Image()
{
	glDeleteTextures( 1, &this->textureID );
}

/**
*/
float Image::getScaling(int maxWidth, int maxHeight)
{
	// assume m.width / m.height is set
	if (!this->m.width || !this->m.height)
		return 1.0f;

	float x_stretch = (float) maxWidth / this->m.width;
    float y_stretch = (float) maxHeight / this->m.height;

    float stretch = (x_stretch < y_stretch) ? x_stretch : y_stretch;
	
	return stretch;
}

/**
*/
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
	float stretch = this->getScaling(maxWidth, maxHeight);

	// new image dimensions
	this->m.width = (unsigned int)((this->m.width * stretch) +.5);
	this->m.height = (unsigned int)((this->m.height * stretch) +.5);

	dib = FreeImage_Rescale(dib, this->m.width,this->m.height, FILTER_BOX);
	// -- save image as texture --

	// FreeImage loads images in BGR format
	dib = FreeImage_ConvertTo32Bits(dib);

#if 0
	FREE_IMAGE_COLOR_TYPE ct = FreeImage_GetColorType(dib);
	unsigned rm = FreeImage_GetRedMask(dib);
	unsigned gm = FreeImage_GetGreenMask(dib);
	unsigned bm = FreeImage_GetBlueMask(dib);
#endif
	// get a pointer to FreeImage's data.
	BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);

	// convert from [BGR] 0xBBGGRR(AA) to [RGB] 0xRRGGBB(AA)
	BYTE tC = 0;
	for(unsigned int pix=0; pix<this->m.width * this->m.height; pix++)
	{
		// Swap RR & BB values
		tC = pixels[pix*4+0];
		pixels[pix*4+0] = pixels[pix*4+2];
		pixels[pix*4+2] = tC;
	}

	glGenTextures( 1, &this->textureID );
	glBindTexture( GL_TEXTURE_2D, this->textureID );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexImage2D( GL_TEXTURE_2D, 0, 4, this->m.width, this->m.height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, pixels );

	FreeImage_Unload(dib);
	dib = 0;

	return 1;
}

/**
*/
void Image::setOld()
{
	isNew = FALSE;
	fade_alpha = 0.0;
}
/**
*/
int Image::Draw(int width, int height, float FPS)
{
	if ( this->x < 1.0 ) this->x += .1f;
	if (this->y < 100.0) this->y += 1.0f;

	if (fade_dur && fade_alpha < 1.0) fade_alpha += (1.0f / (FPS* fade_dur));


	// adjust image scale according to window dimensions
	float xp = 1.0, yp = 1.0;
	float s_aspect = (float)width / (float)height;
	float i_aspect = (float)this->m.width / (float)this->m.height;


#if 0
	// modelview adjustments
	glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();									// reset matrix
	if (i_aspect < s_aspect)
		xp = i_aspect;
	else {
		yp = (s_aspect / i_aspect);
		xp = s_aspect;
	}
	// position quad correctly in Model view
	glTranslatef( 0.0f, 0.0f, -2.40f );
#else
	// projection adjustments
	glMatrixMode (GL_PROJECTION);
    glLoadIdentity();									// reset matrix
	if (i_aspect < s_aspect)
		xp = (i_aspect / s_aspect);
	else
		yp = (s_aspect / i_aspect);
#endif

	glBindTexture( GL_TEXTURE_2D, this->textureID );	// bind texture
//	glTranslatef( g_slideImage->x, g_slideImage->y, -2.4f );
//	glRotatef(this->x,1.0f,0.0f,0.0f);
//	glRotatef(this->y,0.0f,0.0f,1.0f);

	float imgAlpha = (isNew ? fade_alpha : (float)(1.0-fade_alpha));
	glColor4f( 1.0f, 1.0f, 1.0f , imgAlpha );
	
//	glEnable(GL_BLEND);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,0.0); glVertex3f(-xp,-yp,0.0);		//(4:0,1) ___ (3:1,1)
		glTexCoord2f(1.0,0.0); glVertex3f( xp,-yp,0.0);		//       |   |
		glTexCoord2f(1.0,1.0); glVertex3f( xp, yp,0.0);		//       |___|
		glTexCoord2f(0.0,1.0); glVertex3f(-xp, yp,0.0);		//(1:0,0)     (2:1,0)
	glEnd();

	// display some text
#if 0
	if (0 && isNew)
	{
//		glDisable(GL_BLEND);
		glColor4f(1.0f,1.0f,0.0f,1.0f);
//		glRasterPos2f(-1.0f,-0.4f);
//		glTranslatef( -10.0f, -5.0f, 1.0f );
//		glRotatef(this->y, 0.0f, 1.0f, 0.0f);
		glPrint(m.name.c_str());
	}
#endif
	return 0;
}

