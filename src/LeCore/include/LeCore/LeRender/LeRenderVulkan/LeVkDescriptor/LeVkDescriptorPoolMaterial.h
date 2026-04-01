
#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"

class LeVkDescriptorPoolMaterial : public LeVkDescriptorPool
{
public:

	LeVkDescriptorPoolMaterial() {};
	virtual ~LeVkDescriptorPoolMaterial() {};

protected:
	void GetPoolSizes(std::vector<VkDescriptorPoolSize>& PoolSizes) override;
	uint32 GetMaxSets(void) override;
	void GetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& Bindings) override;
};