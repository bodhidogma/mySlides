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

	GLuint getTextureID(){
		return textureID;
	}

	float x, y;

private:
	int Draw();

	Metadata m;
	GLuint textureID;
};

#endif