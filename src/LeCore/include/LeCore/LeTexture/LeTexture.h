
#pragma once

#include "stb_image.h"

#include "LeCore/LeBinary/LeSerializableBinaryChunk.h"

#define TEXTURE_NAME_LENGTH 64

#define TEXTURE_PIXELS_DIRTY		(1<<0)		// Pixels changed and need to be updated
#define TEXTURE_ATTRIBUTES_DIRTY	(1<<1)		// Texture attributes changed and need to be updated

#define TEXTURE_ALL_DIRTY_FLAGS (TEXTURE_PIXELS_DIRTY|TEXTURE_ATTRIBUTES_DIRTY)

enum class LeTextureFilterMode
{
	NONE = 0,
	NEAREST,
	LINEAR
};

enum class LeTextureMipmapMode
{
	NONE = 0,
	NEAREST,
	LINEAR
};

enum class LeTextureAddressMode
{
	NONE = 0,
	REPEAT,     /* UV wraps (tiles)							 */
	MIRROR,     /* Alternate UV is flipped					 */
	CLAMP,      /* UV is clamped to 0-1						 */
	BORDER      /* Border color takes effect outside of 0-1  */
};



struct LeTextureAttributes
{
	LeTextureFilterMode MinFilter = LeTextureFilterMode::LINEAR, MagFilter = LeTextureFilterMode::LINEAR;
	LeTextureMipmapMode MipmapMode = LeTextureMipmapMode::LINEAR;
	LeTextureAddressMode AddressModeU = LeTextureAddressMode::REPEAT, AddressModeV = LeTextureAddressMode::REPEAT;
};


#define LE_TEXTURE_MAX_NAME 128
#define LE_TEXTURE_MAX_SIZE 2048

// Supports only 32bit RGBA textures - super primitive
class LeTexture : public LeSerializableBinaryChunk
{

protected:
	LeTexture();
public:
	virtual ~LeTexture();
	static LeTexture* Create();

	const LeChar* GetName(void) const { return m_name; }
	void SetName(const LeChar* pName);

	uint64 GetNameHash(void) const { return m_nameHash; }

	void AllocatePixels(uint32 width, uint32 height);
	void SetPixels(const LeRGBA* pPixels);
	const LeRGBA* GetPixels(void) const;

	virtual void OnPixelsAllocated(void) = 0;

	uint32 GetWidth(void) const { return m_width; }
	uint32 GetHeight(void) const { return m_height; }

	void SetAttributes(LeTextureAttributes* pAttribs);
	const LeTextureAttributes* GetAttributes(void) { return &m_attribs; }

	LeSerializableBinaryChunkType GetBinaryChunkType(void) { return LeSerializableBinaryChunkType::Texture; }
	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

	uint32 flags;

protected:

	uint32 m_width, m_height;
	LeRGBA* m_pixels;

	LeTextureAttributes m_attribs;

private:

	friend class LeTextureDictionary;

	LeChar m_name[LE_TEXTURE_MAX_NAME];
	uint64 m_nameHash;



};