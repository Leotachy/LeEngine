
#pragma once

// Staging buffer manager that allocates temporary memory buffers that will automatically get cleaned up after being used

class LeVkStagingBufferManager
{

public:
	void Init(VkDevice Device, const LeVkPhysicalDevice* pPhysicalDevice);
	void DeInit(void);

	LeVkMemoryBuffer* MakeBuffer(uint32 FrameIndex, VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties);
	void FreeBuffers(uint32 FrameIndex);


private:

	VkDevice m_device;
	const LeVkPhysicalDevice* m_physicalDevice;

	std::vector<LeVkMemoryBuffer> m_stagingMemoryBuffers[LECORE_VULKAN_MAX_FRAMES_IN_FLIGHT];

};