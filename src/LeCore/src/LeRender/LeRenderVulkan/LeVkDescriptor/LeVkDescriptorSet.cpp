
#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSet.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolManager.h"

LeVkDescriptorSet::LeVkDescriptorSet()
{
	m_device = VK_NULL_HANDLE;
	m_descriptorSet = VK_NULL_HANDLE;
}

void LeVkDescriptorSet::Init(void)
{

	m_device = LeVkRenderer::Get()->GetDevice();

	LeVkDescriptorPool* LeDescriptorPool = LeVkDescriptorPoolManager::Get().GetLeDescriptorPool(GetPoolType());
	LE_DEBUG_ASSERT(LeDescriptorPool);

	VkDescriptorPool DescriptorPool = LeDescriptorPool->GetDescriptorPool();
	VkDescriptorSetLayout DescriptorSetLayout = LeDescriptorPool->GetDescriptorSetLayout();

	CreateSet(DescriptorPool, DescriptorSetLayout);
}

void LeVkDescriptorSet::CreateSet(VkDescriptorPool DescriptorPool, VkDescriptorSetLayout DescriptorSetLayout)
{
	LE_DEBUG_ASSERT(DescriptorPool != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(DescriptorSetLayout != VK_NULL_HANDLE);

	VkDescriptorSetAllocateInfo AllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = DescriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &DescriptorSetLayout
	};


	VkResult res = vkAllocateDescriptorSets(m_device, &AllocInfo, &m_descriptorSet);
	LE_CHECK_VK_RESULT(res);
}