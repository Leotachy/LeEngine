#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSet.h"

class LeVkDescriptorSetMaterial : public LeVkDescriptorSet
{
public:
	LeVkDescriptorSetMaterial(VkDescriptorPool DescriptorPool, uint32 Count);
	virtual ~LeVkDescriptorSetMaterial();

	void UpdateSets(LeVkMemoryBuffer2* ModelMatrices);

protected:

	void GetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& Bindings) override;

};