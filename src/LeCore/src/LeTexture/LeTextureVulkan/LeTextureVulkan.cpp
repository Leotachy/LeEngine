
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeTextureVulkan/LeTextureVulkan.h"

LeVulkanTexture::LeVulkanTexture()
{

    m_device = VK_NULL_HANDLE;

    m_image = VK_NULL_HANDLE;
    m_mem = VK_NULL_HANDLE;
    m_view = VK_NULL_HANDLE;
    m_sampler = VK_NULL_HANDLE;
}

LeVulkanTexture::~LeVulkanTexture()
{

    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroySampler(m_device, m_sampler, NULL);
        vkDestroyImageView(m_device, m_view, NULL);
        vkDestroyImage(m_device, m_image, NULL);
        vkFreeMemory(m_device, m_mem, NULL);
    }


}