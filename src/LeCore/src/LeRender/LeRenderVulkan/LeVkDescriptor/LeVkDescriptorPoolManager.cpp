
#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolManager.h"



LeVkDescriptorPoolManager& LeVkDescriptorPoolManager::Singleton()
{
	static LeVkDescriptorPoolManager DescriptorPoolManager;
	return DescriptorPoolManager;
}

void LeVkDescriptorPoolManager::Init()
{
	m_descriptorPoolGlobal.Init();
	m_descriptorPoolMesh.Init();
	m_descriptorPoolMaterial.Init();
	m_descriptorPoolImGui.Init();
}

void LeVkDescriptorPoolManager::DeInit()
{
	m_descriptorPoolGlobal.DeInit();
	m_descriptorPoolMesh.DeInit();
	m_descriptorPoolMaterial.DeInit();
	m_descriptorPoolImGui.DeInit();
}

LeVkDescriptorPool* LeVkDescriptorPoolManager::GetLeDescriptorPool(LeVkDescriptorPoolType Type)
{
	switch (Type)
	{
	case LeVkDescriptorPoolType::GLOBAL: return &m_descriptorPoolGlobal;
	case LeVkDescriptorPoolType::MATBLOCK: return &m_descriptorPoolMesh;
	case LeVkDescriptorPoolType::MATERIAL: return &m_descriptorPoolMaterial;
	case LeVkDescriptorPoolType::IMGUI: return &m_descriptorPoolImGui;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}