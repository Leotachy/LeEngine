
#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolGlobal.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMaterial.h"

enum LeVkDescriptorPoolType
{
	GLOBAL,
	MATBLOCK,
	MATERIAL,

	NUM_DESCRIPTOR_POOLS
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

};