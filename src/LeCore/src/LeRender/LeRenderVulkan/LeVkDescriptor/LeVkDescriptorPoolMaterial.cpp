#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMaterial.h"


void LeVkDescriptorPoolMaterial::GetPoolSizes(VkDescriptorPoolSize** PoolSizes, uint32* NumSizes)
{
	LE_DEBUG_ASSERT(PoolSizes);
	LE_DEBUG_ASSERT(NumSizes);

	static VkDescriptorPoolSize DescriptorPoolSizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
	};

	*PoolSizes = DescriptorPoolSizes;
	*NumSizes = ARRAY_SIZE(DescriptorPoolSizes);
}

uint32 LeVkDescriptorPoolMaterial::GetMaxSets(void)
{
	return 1000;
}

void LeVkDescriptorPoolMaterial::GetLayoutBindings(VkDescriptorSetLayoutBinding** Bindings, uint32* NumSizes)
{
	LE_DEBUG_ASSERT(Bindings);
	LE_DEBUG_ASSERT(NumSizes);

	static VkDescriptorSetLayoutBinding DescriptorSetLayoutBindings[] =
	{
		{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
	};

	*Bindings = DescriptorSetLayoutBindings;
	*NumSizes = ARRAY_SIZE(DescriptorSetLayoutBindings);
}
