#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMatBlock.h"

LeVkDescriptorPoolMatBlock::LeVkDescriptorPoolMatBlock(uint32 DescriptorCount)
{
	VkDescriptorPoolSize PoolSizes[] =
	{
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = DescriptorCount
			
		}
	};

	VkDescriptorPoolCreateInfo PoolInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = 0,
		.maxSets = DescriptorCount,
		.poolSizeCount = ARRAY_SIZE(PoolSizes),
		.pPoolSizes = PoolSizes
	};


	VkResult res = vkCreateDescriptorPool(m_device, &PoolInfo, NULL, &m_descriptorPool);
	LE_CHECK_VK_RESULT(res, "vkCreateDescriptorPool");
}