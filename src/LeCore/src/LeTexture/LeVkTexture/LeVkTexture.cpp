
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"


VkFilter GetVkFilterForLeFilterMode(LeTextureFilterMode LeFilterMode)
{
    switch (LeFilterMode)
    {
    case LeTextureFilterMode::FILTER_NEAREST:	return VK_FILTER_NEAREST;
    case LeTextureFilterMode::FILTER_LINEAR:	return VK_FILTER_LINEAR;
    }

    LE_DEBUG_ERROR("Unknown LeTextureFilterMode %u", LeFilterMode);
    return VK_FILTER_LINEAR;
}

VkSamplerMipmapMode GetVkSamplerMipmapModeForLeMipmapMode(LeTextureMipmapMode LeMipmapMode)
{
    switch (LeMipmapMode)
    {
    case LeTextureMipmapMode::MIPMAP_NEAREST:	return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case LeTextureMipmapMode::MIPMAP_LINEAR:	return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }

    LE_DEBUG_ERROR("Unknown LeTextureMipmapMode %u", LeMipmapMode);
    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
}

VkSamplerAddressMode GetVkSamplerAddressModeForLeAddressMode(LeTextureAddressMode LeMipmapMode)
{
    switch (LeMipmapMode)
    {
    case ADDRESS_REPEAT: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case ADDRESS_MIRROR: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case ADDRESS_CLAMP: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case ADDRESS_BORDER: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    }

    LE_DEBUG_ERROR("Unknown LeTextureAddressMode %u", LeMipmapMode);
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

LeVkTextureAttributes::LeVkTextureAttributes(const LeTextureAttributes& BaseAttribs)
{
    MinFilter = GetVkFilterForLeFilterMode(BaseAttribs.MinFilter);
    MagFilter = GetVkFilterForLeFilterMode(BaseAttribs.MagFilter);
    MipmapMode = GetVkSamplerMipmapModeForLeMipmapMode(BaseAttribs.MipmapMode);
    AddressModeU = GetVkSamplerAddressModeForLeAddressMode(BaseAttribs.AddressModeU);
    AddressModeV = GetVkSamplerAddressModeForLeAddressMode(BaseAttribs.AddressModeV);
}


LeVkTexture::LeVkTexture(const LeChar* pName) : LeTexture(pName)
{
    device = LeVkCore::Singleton().LeVkGetLogicalDevice();
    physicalDevice = LeVkCore::Singleton().LeVkGetPhysicalDevice();

    view = VK_NULL_HANDLE;
    sampler = VK_NULL_HANDLE;

    m_format = VK_FORMAT_R8G8B8A8_UNORM;
}

LeVkTexture::~LeVkTexture()
{
    ClearPixels();
}

// Creates a staging buffer with host visible memory and
// image with device local memory
// Does not update the image, that must be done during command buffer recording!
void LeVkTexture::SetPixels(const LeRGBA* pPixels, uint32 width, uint32 height)
{
    ClearPixels();

    m_width = width;
    m_height = height;

    // Make a staging buffer: this one will be used as source when transfering to image

    VkDeviceSize imageSize = width * height * LeVkUtils::GetVkBytesPerTexFormat(m_format);

    LeVkUtils::CreateBuffer(device, &physicalDevice->m_memProps, imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &stagingBuffer);
    LeVkUtils::UpdateBuffer(&stagingBuffer, pPixels, imageSize);
    
    // Make the image itself
    LeVkUtils::CreateImage(device, &physicalDevice->m_memProps,
        m_width, m_height, m_format,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &textureImage);
   
    // And the image view :3
    view = LeVkUtils::CreateImageView(device, textureImage.image, m_format,
        VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
    LE_DEBUG_ASSERT(view != VK_NULL_HANDLE);

    // And the sampler
    LeVkTextureAttributes VkTextureAttribs(m_attribs);
    sampler = LeVkUtils::CreateSampler(device, VkTextureAttribs, true, physicalDevice->m_devProps.limits.maxSamplerAnisotropy);
    LE_DEBUG_ASSERT(sampler != VK_NULL_HANDLE);

    AddFlags(TEXTURE_PIXELS_DIRTY);
    DeleteFlags(TEXTURE_ATTRIBUTES_DIRTY);

    LeVkCore::Singleton().QueueTextureForImageUpdate(this);
}

void LeVkTexture::ClearPixels(void)
{
    if (sampler != VK_NULL_HANDLE)
    {
        LeVkUtils::DestroySampler(device, sampler);
        sampler = VK_NULL_HANDLE;
    }

    if (view != VK_NULL_HANDLE)
    {
        LeVkUtils::DestroyImageView(device, view);
        view = VK_NULL_HANDLE;
    }

    if (textureImage.IsAlloced())
        LeVkUtils::DestroyImage(&textureImage);

    if (stagingBuffer.IsAlloced())
        LeVkUtils::DestroyBuffer(&stagingBuffer);
}

LeBool LeVkTexture::ArePixelsSet(void)
{
    return textureImage.image != VK_NULL_HANDLE;
}


