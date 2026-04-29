
#pragma once

#include "stb_image.h"

#include "LeCore/LeBinary/LeBinaryChunk.h"

#define TEXTURE_NAME_LENGTH 64

#define TEXTURE_PIXELS_DIRTY		(1<<0)		// Pixels changed and need to be updated
#define TEXTURE_ATTRIBUTES_DIRTY	(1<<1)		// Texture attributes changed and need to be updated

enum LeTextureFilterMode
{
	FILTER_NONE = 0,
	FILTER_NEAREST,
	FILTER_LINEAR
};

enum LeTextureMipmapMode
{
	MIPMAP_NONE = 0,
	MIPMAP_NEAREST,
	MIPMAP_LINEAR
};

enum LeTextureAddressMode
{
	ADDRESS_NONE = 0,
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


#define LE_TEXTURE_MAX_NAME 128

// Supports only 32bit RGBA textures - super primitive
class LeTexture
{

protected:
	LeTexture(uint32 width, uint32 height);
public:
	virtual ~LeTexture();
	static LeTexture* Create(uint32 width, uint32 height);

	const LeChar* GetName(void) { return m_name; }
	void SetName(const LeChar* pName);

	void SetPixels(const LeRGBA* pPixels);
	void ClearPixels(void);
	LeBool ArePixelsSet(void) const;
	const LeRGBA* GetPixels(void);

	uint32 GetWidth(void) { return m_width; }
	uint32 GetHeight(void) { return m_height; }

	void SetAttributes(LeTextureAttributes* pAttribs);
	const LeTextureAttributes* GetAttributes(void) { return &m_attribs; }

	uint32 flags;

protected:

	uint32 m_width, m_height;
	LeRGBA* m_pixels;

	LeTextureAttributes m_attribs;

private:

	friend class LeTextureDictionary;

	LeChar m_name[LE_TEXTURE_MAX_NAME];



};