/**
*/

#ifndef _IMAGE_H_
#define _IMAGE_H_

struct Metadata
{
	tstring name;
	unsigned int width;
	unsigned int height;
};

class Image
{
public:
	Image(tstring imageName, int maxWidth, int maxHeight);
	~Image();

	int Draw(int width, int height, float FPS);
	void setOld();

	float fade_alpha;

private:
	int loadImageTexture(tstring imageName, int maxWidth, int maxHeight);
	float getScaling(int maxWidth,int maxHeight);
	GLuint getTextureID(){ return textureID; }

	GLuint textureID;
	Metadata m;
	float x, y;
	BOOL isNew;
};

#endif