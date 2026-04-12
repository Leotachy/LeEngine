
#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"

class LeVkDescriptorPoolMaterial : public LeVkDescriptorPool
{
public:

	LeVkDescriptorPoolMaterial() {};
	virtual ~LeVkDescriptorPoolMaterial() {};

protected:
	void GetPoolSizes(VkDescriptorPoolSize** PoolSizes, uint32* NumSizes) override;
	uint32 GetMaxSets(void) override;
	void GetLayoutBindings(VkDescriptorSetLayoutBinding** Bindings, uint32* NumSizes) override;
};