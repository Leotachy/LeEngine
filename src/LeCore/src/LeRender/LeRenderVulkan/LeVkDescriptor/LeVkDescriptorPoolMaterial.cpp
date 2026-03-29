#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMaterial.h"

LeVkDescriptorPoolMaterial::LeVkDescriptorPoolMaterial(uint32 DescriptorCount)
{
	VkDescriptorPoolSize PoolSizes[] =
	{
		{
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
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
	LE_CHECK_VK_RESULT(res);
}