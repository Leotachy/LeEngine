#pragma once


#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSet.h"

class LeVkDescriptorSetGlobal : public LeVkDescriptorSet
{
public:
	LeVkDescriptorSetGlobal(VkDescriptorPool DescriptorPool, uint32 Count);
	~LeVkDescriptorSetGlobal();


	void UpdateSets(LeVkMemoryBuffer2* MvpMatrices);

protected:

	void GetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& Bindings) override;

};