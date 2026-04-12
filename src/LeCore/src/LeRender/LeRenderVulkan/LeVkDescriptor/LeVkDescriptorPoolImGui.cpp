#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolImGui.h"


void LeVkDescriptorPoolImGui::GetPoolSizes(VkDescriptorPoolSize** PoolSizes, uint32* NumSizes)
{

	LE_DEBUG_ASSERT(PoolSizes);
	LE_DEBUG_ASSERT(NumSizes);

	static VkDescriptorPoolSize DescriptorPoolSizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	*PoolSizes = DescriptorPoolSizes;
	*NumSizes = ARRAY_SIZE(DescriptorPoolSizes);
}

uint32  LeVkDescriptorPoolImGui::GetMaxSets(void)
{
	return 11000;
}

void  LeVkDescriptorPoolImGui::GetLayoutBindings(VkDescriptorSetLayoutBinding** Bindings, uint32* NumSizes)
{

	LE_DEBUG_ASSERT(Bindings);
	LE_DEBUG_ASSERT(NumSizes);

	*Bindings = NULL;
	*NumSizes = 0;
}
