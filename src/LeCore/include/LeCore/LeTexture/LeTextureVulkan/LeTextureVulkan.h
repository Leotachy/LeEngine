
#pragma once

#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"

class LeVulkanTexture : public LeTexture
{
public:
    LeVulkanTexture();
    ~LeVulkanTexture();

private:

    VkDevice m_device;

    VkImage m_image;
    VkDeviceMemory m_mem;
    VkImageView m_view;
    VkSampler m_sampler;
};