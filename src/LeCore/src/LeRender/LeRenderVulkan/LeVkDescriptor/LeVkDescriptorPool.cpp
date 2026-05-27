
#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolGlobal.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMaterial.h"


LeVkDescriptorPool::LeVkDescriptorPool()
{
	m_hDevice = VK_NULL_HANDLE;
	m_descriptorPool = VK_NULL_HANDLE;
	m_descriptorSetLayout = VK_NULL_HANDLE;
}

LeVkDescriptorPool::~LeVkDescriptorPool()
{
	DeInit();
}

void LeVkDescriptorPool::Init(void)
{

	m_hDevice = LeVkRenderer::Get()->GetDevice();

	VkDescriptorPoolSize* PoolSizes;
	uint32 NumPoolSizes;

	GetPoolSizes(&PoolSizes, &NumPoolSizes);

	LE_DEBUG_ASSERT(PoolSizes != NULL);
	LE_DEBUG_ASSERT(NumPoolSizes > 0);

	VkDescriptorPoolCreateInfo PoolInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = GetMaxSets(),
		.poolSizeCount = NumPoolSizes,
		.pPoolSizes = PoolSizes
	};

	VkResult res = vkCreateDescriptorPool(m_hDevice, &PoolInfo, NULL, &m_descriptorPool);
	LE_CHECK_VK_RESULT(res);


	VkDescriptorSetLayoutBinding* Bindings;
	uint32 NumBindingsSizes;

	GetLayoutBindings(&Bindings, &NumBindingsSizes);

	if (NumBindingsSizes > 0)
	{
		VkDescriptorSetLayoutCreateInfo LayoutInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = NumBindingsSizes,
			.pBindings = Bindings
		};

		res = vkCreateDescriptorSetLayout(m_hDevice, &LayoutInfo, NULL, &m_descriptorSetLayout);
		LE_CHECK_VK_RESULT(res);
	}
}

void LeVkDescriptorPool::DeInit(void)
{
	if (m_descriptorSetLayout != VK_NULL_HANDLE)
	{
		LE_DEBUG_ASSERT(m_hDevice != VK_NULL_HANDLE);
		vkDestroyDescriptorSetLayout(m_hDevice, m_descriptorSetLayout, NULL);
		m_descriptorSetLayout = VK_NULL_HANDLE;
	}

	if (m_descriptorPool)
	{
		LE_DEBUG_ASSERT(m_hDevice != VK_NULL_HANDLE);
		vkDestroyDescriptorPool(m_hDevice, m_descriptorPool, NULL);
		m_descriptorPool = VK_NULL_HANDLE;
	}

	m_hDevice = VK_NULL_HANDLE;
}

VkDescriptorPool LeVkDescriptorPool::GetDescriptorPool(void) const
{
	return m_descriptorPool;
}

VkDescriptorSetLayout LeVkDescriptorPool::GetDescriptorSetLayout(void) const
{
	return m_descriptorSetLayout;
}