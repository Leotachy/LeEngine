
#pragma once

#include "stb_image.h"

#define TEXTURE_NAME_LENGTH 64

// Supports only 32bit RGBA textures - super primitive

#include "LeCore/LeRender/LeRenderResource.h"


class LeTexture : public LeRenderResource
{

protected:
	LeTexture();
public:
	virtual ~LeTexture();
	static LeTexture* Create(void);

	void SetPixels(const LeRGBA* pPixels, int32 width, int32 height);

private:

	LeChar m_name[TEXTURE_NAME_LENGTH];
	LeRGBA* m_pixels;
	int32 m_width, m_height;



};