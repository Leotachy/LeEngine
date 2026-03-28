#pragma once


class LeVkDescriptorSet
{
public:
	LeVkDescriptorSet(VkDescriptorPool DescriptorPool, uint32 Count);
	virtual ~LeVkDescriptorSet();

protected:

	virtual void GetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& Bindings) = 0;

	VkDevice m_device;

	VkDescriptorSetLayout m_descriptorSetLayout;
	std::vector<VkDescriptorSet> m_descriptorSets;


private:

	void CreateLayout();
	void CreateSets(VkDescriptorPool DescriptorPool, uint32 Count);
};