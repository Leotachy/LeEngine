

#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"

class LeVkDescriptorPoolManager
{
public:
	static LeVkDescriptorPoolManager* Singleton();

	void Init(uint32 NumImages);
	void DeInit();

	LeVkDescriptorPool* GetDescriptorPool(LeVkDescriptorPoolType Type);

private:

	uint32 m_numImages;
	LeVkDescriptorPool* m_descriptorPools[LeVkDescriptorPoolType::NUM_DESCRIPTOR_POOLS];
};