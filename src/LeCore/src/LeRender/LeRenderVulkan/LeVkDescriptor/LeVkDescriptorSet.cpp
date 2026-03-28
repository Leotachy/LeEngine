
#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSet.h"

LeVkDescriptorSet::LeVkDescriptorSet(VkDescriptorPool DescriptorPool, uint32 Count)
{

	m_device = LeVkGetLogicalDevice();

	m_descriptorSetLayout = VK_NULL_HANDLE;
	m_descriptorSets.clear();

	CreateLayout();
	CreateSets(DescriptorPool, Count);
}

LeVkDescriptorSet::~LeVkDescriptorSet()
{
	if (m_descriptorSetLayout != VK_NULL_HANDLE)
	{
		LE_DEBUG_ASSERT(m_device != VK_NULL_HANDLE);

		vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, NULL);
	}
}

void LeVkDescriptorSet::CreateLayout(void)
{
	std::vector<VkDescriptorSetLayoutBinding> Bindings;
	GetLayoutBindings(Bindings);

	VkDescriptorSetLayoutCreateInfo LayoutInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = Bindings.size(),
		.pBindings = Bindings.data()
	};

	VkResult res = vkCreateDescriptorSetLayout(m_device, &LayoutInfo, NULL, &m_descriptorSetLayout);
	LE_CHECK_VK_RESULT(res);
}

void LeVkDescriptorSet::CreateSets(VkDescriptorPool DescriptorPool, uint32 Count)
{
	std::vector<VkDescriptorSetLayout> Layouts(Count, m_descriptorSetLayout);

	VkDescriptorSetAllocateInfo AllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = DescriptorPool,
		.descriptorSetCount = Count,
		.pSetLayouts = Layouts.data()
	};

	m_descriptorSets.resize(Count);
	VkResult res = vkAllocateDescriptorSets(m_device, &AllocInfo, m_descriptorSets.data());
	LE_CHECK_VK_RESULT(res);
}