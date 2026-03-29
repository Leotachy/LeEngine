
#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"


LeVkDescriptorPool::LeVkDescriptorPool()
{
	m_type = LeVkDescriptorPoolType::NONE;
	m_device = LeVkCore::Singleton().LeVkGetLogicalDevice();
	m_descriptorPool = VK_NULL_HANDLE;
}

LeVkDescriptorPool::~LeVkDescriptorPool()
{
	if (m_descriptorPool)
	{
		LE_DEBUG_ASSERT(m_device != VK_NULL_HANDLE);
		vkDestroyDescriptorPool(m_device, m_descriptorPool, NULL);
	}
}