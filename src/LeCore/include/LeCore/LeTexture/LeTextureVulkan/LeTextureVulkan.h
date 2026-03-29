
#pragma once

#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryImage.h"


VkFilter GetVkFilterForLeFilterMode(LeTextureFilterMode LeFilterMode);
VkSamplerMipmapMode GetVkSamplerMipmapModeForLeMipmapMode(LeTextureMipmapMode LeMipmapMode);
VkSamplerAddressMode GetVkSamplerAddressModeForLeAddressMode(LeTextureAddressMode LeMipmapMode);


struct LeVkTextureAttributes
{

    LeVkTextureAttributes(const LeTextureAttributes& BaseAttribs);

    VkFilter MinFilter, MagFilter;
    VkSamplerMipmapMode MipmapMode;
    VkSamplerAddressMode AddressModeU, AddressModeV;

};



class LeTextureVk : public LeTexture
{
public:
    LeTextureVk();
    ~LeTextureVk();


    virtual void SetPixels(const LeRGBA* pPixels, uint32 width, uint32 height) override;
    virtual void ClearPixels(void) override;
    virtual LeBool ArePixelsSet(void) override;

    VkFormat GetVkFormat(void) const { return m_format; }

    VkDevice device;
    const LeVkPhysicalDevice* physicalDevice;

    LeVkMemoryBuffer stagingBuffer;
    LeVkMemoryImage textureImage;
    VkImageView view;
    VkSampler sampler;


private:



    VkFormat m_format;


};
