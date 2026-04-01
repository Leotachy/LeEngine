#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMesh.h"

void LeVkDescriptorPoolMesh::GetPoolSizes(std::vector<VkDescriptorPoolSize>& PoolSizes)
{
	PoolSizes.emplace_back(VkDescriptorPoolSize
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1
		});
}

uint32 LeVkDescriptorPoolMesh::GetMaxSets(void)
{
	return 5000;
}


void LeVkDescriptorPoolMesh::GetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& Bindings)
{

	Bindings.emplace_back(VkDescriptorSetLayoutBinding{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		}
	);
}
