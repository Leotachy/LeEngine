#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"

class LeVkDescriptorPoolMesh : public LeVkDescriptorPool
{
public:

	LeVkDescriptorPoolMesh() {};
	virtual ~LeVkDescriptorPoolMesh() {};

protected:
	void GetPoolSizes(VkDescriptorPoolSize** PoolSizes, uint32* NumSizes) override;
	uint32 GetMaxSets(void) override;
	void GetLayoutBindings(VkDescriptorSetLayoutBinding** Bindings, uint32* NumSizes) override;
};