
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"


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


LeVkTexture::LeVkTexture()
{
    m_format = VK_FORMAT_R8G8B8A8_UNORM;

    allocatedImage = {};
    sampler = VK_NULL_HANDLE;
}

LeVkTexture::~LeVkTexture()
{

    if (allocatedImage.allocation)
        LeVkUtils::DestroyImage(&allocatedImage);
}

void LeVkTexture::OnPixelsAllocated(void)
{

    VkExtent3D extent = VkExtent3D(m_Width, m_Height, 1);

    LeVkUtils::CreateImage(extent, m_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_VIEW_TYPE_2D,
        1, 1,
        &allocatedImage);
}



VkFilter GetVkFilterForLeFilterMode(LeTextureFilterMode LeFilterMode)
{
    switch (LeFilterMode)
    {
    case LeTextureFilterMode::NEAREST:	return VK_FILTER_NEAREST;
    case LeTextureFilterMode::LINEAR:	return VK_FILTER_LINEAR;
    }

    LE_DEBUG_ERROR("Unknown LeTextureFilterMode %u", LeFilterMode);
    return VK_FILTER_LINEAR;
}

VkSamplerMipmapMode GetVkSamplerMipmapModeForLeMipmapMode(LeTextureMipmapMode LeMipmapMode)
{
    switch (LeMipmapMode)
    {
    case LeTextureMipmapMode::NEAREST:	return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case LeTextureMipmapMode::LINEAR:	return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }

    LE_DEBUG_ERROR("Unknown LeTextureMipmapMode %u", LeMipmapMode);
    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
}

VkSamplerAddressMode GetVkSamplerAddressModeForLeAddressMode(LeTextureAddressMode LeMipmapMode)
{
    switch (LeMipmapMode)
    {
    case LeTextureAddressMode::REPEAT: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case LeTextureAddressMode::MIRROR: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case LeTextureAddressMode::CLAMP: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case LeTextureAddressMode::BORDER: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    }

    LE_DEBUG_ERROR("Unknown LeTextureAddressMode %u", LeMipmapMode);
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}