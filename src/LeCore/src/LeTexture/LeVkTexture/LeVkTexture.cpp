
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

LeVkTextureAttributes::LeVkTextureAttributes()
{
    MagFilter = MinFilter = VK_FILTER_LINEAR;
    MipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    AddressModeV = AddressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

LeVkTextureAttributes::LeVkTextureAttributes(const LeTextureAttributes* BaseAttribs)
{
    MinFilter = GetVkFilterForLeFilterMode(BaseAttribs->MinFilter);
    MagFilter = GetVkFilterForLeFilterMode(BaseAttribs->MagFilter);
    MipmapMode = GetVkSamplerMipmapModeForLeMipmapMode(BaseAttribs->MipmapMode);
    AddressModeU = GetVkSamplerAddressModeForLeAddressMode(BaseAttribs->AddressModeU);
    AddressModeV = GetVkSamplerAddressModeForLeAddressMode(BaseAttribs->AddressModeV);
}


LeVkTexture::LeVkTexture(uint32 width, uint32 height) : LeTexture(width, height)
{
    device = LeVkCore::Singleton().GetDevice();
    physicalDevice = LeVkCore::Singleton().GetPhysicalDevice();
    m_format = VK_FORMAT_R8G8B8A8_UNORM;


    VkDeviceSize imageSize = m_width * m_height * LeVkUtils::GetVkBytesPerTexFormat(m_format);

    // Make the image itself
    LeVkUtils::CreateImage(device, &physicalDevice->m_memProps,
        m_width, m_height, m_format,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &textureImage);

    // And the image view :3
    view = LeVkUtils::CreateImageView(device, textureImage.image, m_format,
        VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
    LE_DEBUG_ASSERT(view != VK_NULL_HANDLE);

    AddFlags(TEXTURE_PIXELS_DIRTY);
    DeleteFlags(TEXTURE_ATTRIBUTES_DIRTY);
}

LeVkTexture::~LeVkTexture()
{
    if (view != VK_NULL_HANDLE)
    {
        LeVkUtils::DestroyImageView(device, view);
        view = VK_NULL_HANDLE;
    }

    if (textureImage.IsAlloced())
        LeVkUtils::DestroyImage(&textureImage);
}
