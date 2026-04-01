#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolGlobal.h"



void LeVkDescriptorPoolGlobal::GetPoolSizes(std::vector<VkDescriptorPoolSize>& PoolSizes)
{

	PoolSizes.emplace_back(VkDescriptorPoolSize
	{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 2
	});
}

uint32 LeVkDescriptorPoolGlobal::GetMaxSets(void)
{
	return 1;
}

void LeVkDescriptorPoolGlobal::GetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& Bindings)
{

	Bindings.emplace_back(VkDescriptorSetLayoutBinding{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		}
	);
}
