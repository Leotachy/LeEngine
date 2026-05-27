#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolManager.h"

class LeVkDescriptorSet
{
public:
	LeVkDescriptorSet();
	virtual ~LeVkDescriptorSet() {};

	void Init(void);

	virtual void UpdateSet(void* pData) = 0;

	VkDescriptorSet GetDescriptorSet(void) { return m_DescriptorSet; };

protected:
	virtual LeVkDescriptorPoolType GetPoolType(void) = 0;

	VkDevice m_hDevice;
	VkDescriptorSet m_DescriptorSet;

private:

	void CreateSet(VkDescriptorPool DescriptorPool, VkDescriptorSetLayout DescriptorSetLayout);
};