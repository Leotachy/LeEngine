#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"

class LeVkDescriptorSet
{
public:
	LeVkDescriptorSet();
	virtual ~LeVkDescriptorSet() {};

	virtual void UpdateSet(void* pData) = 0;

	VkDescriptorSet GetDescriptorSet(void) { return m_descriptorSet; };

protected:
	virtual LeVkDescriptorPoolType GetPoolType(void) = 0;

	VkDevice m_device;
	VkDescriptorSet m_descriptorSet;

private:

	void CreateSet(VkDescriptorPool DescriptorPool, VkDescriptorSetLayout DescriptorSetLayout);
};