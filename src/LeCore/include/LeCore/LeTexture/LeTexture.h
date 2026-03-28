
#pragma once

#include "stb_image.h"
#include "LeCore/LeRender/LeRenderResource.h"

#define TEXTURE_NAME_LENGTH 64

#define TEXTURE_PIXELS_DIRTY		(1<<0)		// Pixels changed and need to be updated
#define TEXTURE_ATTRIBUTES_DIRTY	(1<<1)		// Texture attributes changed and need to be updated

enum LeTextureFilterMode
{
	NONE = 0,
	FILTER_NEAREST,
	FILTER_LINEAR
};

enum LeTextureMipmapMode
{
	NONE = 0,
	MIPMAP_NEAREST,
	MIPMAP_LINEAR
};

enum LeTextureAddressMode
{
	NONE = 0,
	ADDRESS_REPEAT,     /* UV wraps (tiles)							 */
	ADDRESS_MIRROR,     /* Alternate UV is flipped					 */
	ADDRESS_CLAMP,      /* UV is clamped to 0-1						 */
	ADDRESS_BORDER      /* Border color takes effect outside of 0-1  */
};



struct LeTextureAttributes
{
	LeTextureFilterMode MinFilter = FILTER_LINEAR, MagFilter = FILTER_LINEAR;
	LeTextureMipmapMode MipmapMode = MIPMAP_LINEAR;
	LeTextureAddressMode AddressModeU = ADDRESS_REPEAT, AddressModeV = ADDRESS_REPEAT;
};


// Supports only 32bit RGBA textures - super primitive
class LeTexture : public LeRenderResource
{

protected:
	LeTexture();
public:
	virtual ~LeTexture();
	static LeTexture* Create(void);

	virtual void SetPixels(const LeRGBA* pPixels, uint32 width, uint32 height) = 0;
	virtual void ClearPixels(void) = 0;
	virtual LeBool ArePixelsSet(void) = 0;

	uint32 GetWidth(void) { return m_width; }
	uint32 GetHeight(void) { return m_height; }

protected:

	uint32 m_width, m_height;

	LeTextureAttributes m_attribs;

private:

	LeChar m_name[TEXTURE_NAME_LENGTH];



};