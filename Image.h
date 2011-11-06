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
	Image(tstring imageName, int maxWidth, int maxHeight);
	~Image();

	int Draw(int width, int height);

private:
	int loadImageTexture(tstring imageName, int maxWidth, int maxHeight);
	float getScaling(int maxWidth,int maxHeight);
	GLuint getTextureID(){ return textureID; }

	GLuint textureID;
	Metadata m;
	float x, y;
};

#endif