
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
    LeVkTexture(uint32 width, uint32 height);
    ~LeVkTexture();

    VkFormat GetVkFormat(void) const { return m_format; }

    VkDevice device;
    const LeVkPhysicalDevice* physicalDevice;

    LeVkMemoryImage memoryImage;
    VkImageView view;
    VkSampler sampler;

private:

    VkFormat m_format;


};



VkFilter GetVkFilterForLeFilterMode(LeTextureFilterMode LeFilterMode);
VkSamplerMipmapMode GetVkSamplerMipmapModeForLeMipmapMode(LeTextureMipmapMode LeMipmapMode);
VkSamplerAddressMode GetVkSamplerAddressModeForLeAddressMode(LeTextureAddressMode LeMipmapMode);

