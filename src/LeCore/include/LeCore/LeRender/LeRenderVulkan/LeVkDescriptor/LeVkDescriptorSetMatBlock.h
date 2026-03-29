#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSet.h"

class LeVkDescriptorSetMatBlock : public LeVkDescriptorSet
{
public:
	LeVkDescriptorSetMatBlock(VkDescriptorPool DescriptorPool, uint32 Count);
	virtual ~LeVkDescriptorSetMatBlock();

	void UpdateSets(LeVkMemoryBuffer* ModelMatrices);

protected:

	void GetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& Bindings) override;

};