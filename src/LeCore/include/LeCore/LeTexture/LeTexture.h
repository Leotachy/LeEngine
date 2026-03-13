
#pragma once

#include "stb_image.h"

#define TEXTURE_NAME_LENGTH 64

class LeTexture
{

public:

	LeTexture();
	~LeTexture();



private:

	LeChar m_name[TEXTURE_NAME_LENGTH];
	LeByte* m_pixels;
	int32 m_width, m_height;



};