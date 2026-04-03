
#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolGlobal.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMaterial.h"


LeVkDescriptorPool::LeVkDescriptorPool()
{
	m_device = VK_NULL_HANDLE;
	m_descriptorPool = VK_NULL_HANDLE;
	m_descriptorSetLayout = VK_NULL_HANDLE;
}

LeVkDescriptorPool::~LeVkDescriptorPool()
{
	DeInit();
}

void LeVkDescriptorPool::Init(void)
{

	m_device = LeVkCore::Singleton().LeVkGetLogicalDevice();

	std::vector<VkDescriptorPoolSize> PoolSizes;
	GetPoolSizes(PoolSizes);

	VkDescriptorPoolCreateInfo PoolInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = 0,
		.maxSets = GetMaxSets(),
		.poolSizeCount = ARRAY_SIZE(PoolSizes),
		.pPoolSizes = PoolSizes.data()
	};

	VkResult res = vkCreateDescriptorPool(m_device, &PoolInfo, NULL, &m_descriptorPool);
	LE_CHECK_VK_RESULT(res);


	std::vector<VkDescriptorSetLayoutBinding> Bindings;
	GetLayoutBindings(Bindings);

	VkDescriptorSetLayoutCreateInfo LayoutInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = uint32(Bindings.size()),
		.pBindings = Bindings.data()
	};

	res = vkCreateDescriptorSetLayout(m_device, &LayoutInfo, NULL, &m_descriptorSetLayout);
	LE_CHECK_VK_RESULT(res);
}

void LeVkDescriptorPool::DeInit(void)
{
	if (m_descriptorSetLayout != VK_NULL_HANDLE)
	{
		LE_DEBUG_ASSERT(m_device != VK_NULL_HANDLE);
		vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, NULL);
		m_descriptorSetLayout = VK_NULL_HANDLE;
	}

	if (m_descriptorPool)
	{
		LE_DEBUG_ASSERT(m_device != VK_NULL_HANDLE);
		vkDestroyDescriptorPool(m_device, m_descriptorPool, NULL);
		m_descriptorPool = VK_NULL_HANDLE;
	}

	m_device = VK_NULL_HANDLE;
}

VkDescriptorPool LeVkDescriptorPool::GetDescriptorPool(void) const
{
	return m_descriptorPool;
}

VkDescriptorSetLayout LeVkDescriptorPool::GetDescriptorSetLayout(void) const
{
	return m_descriptorSetLayout;
}










LeVkDescriptorPoolManager& LeVkDescriptorPoolManager::Singleton()
{
	static LeVkDescriptorPoolManager DescriptorPoolManager;
	return DescriptorPoolManager;
}

void LeVkDescriptorPoolManager::Init()
{
	m_descriptorPools[LeVkDescriptorPoolType::GLOBAL] = new LeVkDescriptorPoolGlobal();
	m_descriptorPools[LeVkDescriptorPoolType::GLOBAL]->Init();

	m_descriptorPools[LeVkDescriptorPoolType::MATBLOCK] = new LeVkDescriptorPoolMesh();
	m_descriptorPools[LeVkDescriptorPoolType::MATBLOCK]->Init();

	m_descriptorPools[LeVkDescriptorPoolType::MATERIAL] = new LeVkDescriptorPoolMaterial();
	m_descriptorPools[LeVkDescriptorPoolType::MATERIAL]->Init();
}

void LeVkDescriptorPoolManager::DeInit()
{
	delete m_descriptorPools[LeVkDescriptorPoolType::GLOBAL];
	m_descriptorPools[LeVkDescriptorPoolType::GLOBAL] = NULL;

	delete m_descriptorPools[LeVkDescriptorPoolType::MATBLOCK];
	m_descriptorPools[LeVkDescriptorPoolType::MATBLOCK] = NULL;

	delete m_descriptorPools[LeVkDescriptorPoolType::MATERIAL];
	m_descriptorPools[LeVkDescriptorPoolType::MATERIAL] = NULL;
}

LeVkDescriptorPool* LeVkDescriptorPoolManager::GetLeDescriptorPool(LeVkDescriptorPoolType Type)
{
	LE_DEBUG_ASSERT(Type < LeVkDescriptorPoolType::NUM_DESCRIPTOR_POOLS);

	return m_descriptorPools[Type];
}