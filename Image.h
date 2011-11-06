/**
*/

#ifndef _IMAGE_H_
#define _IMAGE_H_

struct Metadata
{
	unsigned int width;
	unsigned int height;
};

class Image
{
public:
	Image();
	~Image();

	int loadImageTexture(tstring imageName, int maxWidth, int maxHeight);
	int Draw(int width, int height);

	GLuint getTextureID(){ return textureID; }

private:
	float getScaling(int maxWidth,int maxHeight);

	GLuint textureID;
	Metadata m;
	float x, y;
};

#endif