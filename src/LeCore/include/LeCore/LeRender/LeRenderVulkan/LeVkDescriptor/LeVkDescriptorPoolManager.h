
#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolGlobal.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMaterial.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolImGui.h"

enum class LeVkDescriptorPoolType
{
	GLOBAL,
	MATBLOCK,
	MATERIAL,

	NUM_DESCRIPTOR_POOLS,

	IMGUI
};

class LeVkDescriptorPoolManager
{
public:
	static LeVkDescriptorPoolManager& Singleton();

	void Init();
	void DeInit();

	LeVkDescriptorPool* GetLeDescriptorPool(LeVkDescriptorPoolType Type);

private:

	LeVkDescriptorPoolGlobal m_descriptorPoolGlobal;
	LeVkDescriptorPoolMesh m_descriptorPoolMesh;
	LeVkDescriptorPoolMaterial m_descriptorPoolMaterial;
	LeVkDescriptorPoolImGui m_descriptorPoolImGui;

};