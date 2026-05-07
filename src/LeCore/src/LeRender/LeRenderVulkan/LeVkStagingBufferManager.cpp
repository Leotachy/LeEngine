
#include "LeCore/lecore.h"

#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkStagingBufferManager.h"


void LeVkStagingBufferManager::Init(VkDevice Device, const LeVkPhysicalDevice* pPhysicalDevice)
{
	LE_DEBUG_ASSERT(Device != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(pPhysicalDevice);

	m_device = Device;
	m_physicalDevice = pPhysicalDevice;
}

void LeVkStagingBufferManager::DeInit(void)
{
	for (uint32 i = 0; i < LECORE_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
		FreeBuffers(i);

	m_device = VK_NULL_HANDLE;
	m_physicalDevice = NULL;
}

LeVkMemoryBuffer* LeVkStagingBufferManager::MakeBuffer(uint32 FrameIndex,
	VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties)
{
	LE_DEBUG_ASSERT(FrameIndex < LECORE_VULKAN_MAX_FRAMES_IN_FLIGHT);

	m_stagingMemoryBuffers[FrameIndex].push_back(LeVkMemoryBuffer());
	LeVkMemoryBuffer* pMemoryBuffer = &m_stagingMemoryBuffers[FrameIndex].back();

	LeVkUtils::CreateBuffer(m_device, &m_physicalDevice->m_memProps,
		Size, Usage, Properties, pMemoryBuffer);

}

void LeVkStagingBufferManager::FreeBuffers(uint32 FrameIndex)
{
	LE_DEBUG_ASSERT(FrameIndex < LECORE_VULKAN_MAX_FRAMES_IN_FLIGHT);

	for (uint32 i = 0; i < m_stagingMemoryBuffers[FrameIndex].size(); i++)
	{
		LeVkMemoryBuffer* pMemoryBuffer = &m_stagingMemoryBuffers[FrameIndex][i];
		LeVkUtils::DestroyBuffer(pMemoryBuffer);
	}

	m_stagingMemoryBuffers[FrameIndex].clear();
}
