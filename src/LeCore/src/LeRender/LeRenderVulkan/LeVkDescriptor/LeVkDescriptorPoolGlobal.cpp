#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolGlobal.h"

void LeVkDescriptorPoolGlobal::GetPoolSizes(VkDescriptorPoolSize** PoolSizes, uint32* NumSizes)
{

	LE_DEBUG_ASSERT(PoolSizes);
	LE_DEBUG_ASSERT(NumSizes);

	static VkDescriptorPoolSize DescriptorPoolSizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 }
	};

	*PoolSizes = DescriptorPoolSizes;
	*NumSizes = ARRAY_SIZE(DescriptorPoolSizes);
}

uint32 LeVkDescriptorPoolGlobal::GetMaxSets(void)
{
	return 2;
}

void LeVkDescriptorPoolGlobal::GetLayoutBindings(VkDescriptorSetLayoutBinding** Bindings, uint32* NumSizes)
{
	LE_DEBUG_ASSERT(Bindings);
	LE_DEBUG_ASSERT(NumSizes);

	static VkDescriptorSetLayoutBinding DescriptorSetLayoutBindings[] =
	{
		{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT }
	};

	*Bindings = DescriptorSetLayoutBindings;
	*NumSizes = ARRAY_SIZE(DescriptorSetLayoutBindings);
}
