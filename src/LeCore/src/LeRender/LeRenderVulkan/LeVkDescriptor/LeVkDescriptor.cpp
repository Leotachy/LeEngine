

#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptor.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolGlobal.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMatBlock.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMaterial.h"



LeVkDescriptorPoolManager* LeVkDescriptorPoolManager::Singleton()
{
	static LeVkDescriptorPoolManager DescriptorPoolManager;
	return &DescriptorPoolManager;
}

void LeVkDescriptorPoolManager::Init(uint32 NumImages)
{
	LE_DEBUG_ASSERT(NumImages > 0);
	m_numImages = NumImages;

	m_descriptorPools[LeVkDescriptorPoolType::GLOBAL]	= new LeVkDescriptorPoolGlobal(NumImages);
	m_descriptorPools[LeVkDescriptorPoolType::MATBLOCK] = new LeVkDescriptorPoolMatBlock(NumImages);
	m_descriptorPools[LeVkDescriptorPoolType::MATERIAL] = new LeVkDescriptorPoolMaterial(NumImages);
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

LeVkDescriptorPool* LeVkDescriptorPoolManager::GetDescriptorPool(LeVkDescriptorPoolType Type)
{
	LE_DEBUG_ASSERT(Type != LeVkDescriptorPoolType::NONE && Type != LeVkDescriptorPoolType::NUM_DESCRIPTOR_POOLS);

	return m_descriptorPools[Type];
}