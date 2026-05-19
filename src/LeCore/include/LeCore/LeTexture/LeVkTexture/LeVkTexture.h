
#pragma once

#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkPhysicalDevice.h"

struct LeVkTextureAttributes
{
    LeVkTextureAttributes();
    LeVkTextureAttributes(const LeTextureAttributes* BaseAttribs);

    VkFilter MinFilter, MagFilter;
    VkSamplerMipmapMode MipmapMode;
    VkSamplerAddressMode AddressModeU, AddressModeV;

};



class LeVkTexture : public LeTexture
{
public:
    LeVkTexture();
    ~LeVkTexture();

    VkFormat GetVkFormat(void) const { return m_format; }

    void OnPixelsAllocated(void) override;

    LeVkMemoryImage memoryImage;
    VkImageView view;
    VkSampler sampler;

private:

    VkFormat m_format;


};



VkFilter GetVkFilterForLeFilterMode(LeTextureFilterMode LeFilterMode);
VkSamplerMipmapMode GetVkSamplerMipmapModeForLeMipmapMode(LeTextureMipmapMode LeMipmapMode);
VkSamplerAddressMode GetVkSamplerAddressModeForLeAddressMode(LeTextureAddressMode LeMipmapMode);

