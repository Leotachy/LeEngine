
#pragma once

#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryImage.h"


VkFilter GetVkFilterForLeFilterMode(LeTextureFilterMode LeFilterMode);
VkSamplerMipmapMode GetVkSamplerMipmapModeForLeMipmapMode(LeTextureMipmapMode LeMipmapMode);
VkSamplerAddressMode GetVkSamplerAddressModeForLeAddressMode(LeTextureAddressMode LeMipmapMode);


struct LeVkTextureAttributes
{
    LeVkTextureAttributes();
    LeVkTextureAttributes(const LeTextureAttributes& BaseAttribs);

    VkFilter MinFilter, MagFilter;
    VkSamplerMipmapMode MipmapMode;
    VkSamplerAddressMode AddressModeU, AddressModeV;

};



class LeVkTexture : public LeTexture
{
public:
    LeVkTexture();
    ~LeVkTexture();


    void Compile(void) override;

    VkFormat GetVkFormat(void) const { return m_format; }

    VkDevice device;
    const LeVkPhysicalDevice* physicalDevice;

    LeVkMemoryBuffer stagingBuffer;
    LeVkMemoryImage textureImage;
    VkImageView view;
    VkSampler sampler;


private:

    void DestroyVulkanResources(void);

    VkFormat m_format;


};
