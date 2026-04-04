
#pragma once


// Base class for LeVkDescriptorPool

class LeVkDescriptorPool
{
public:
	LeVkDescriptorPool();
	virtual ~LeVkDescriptorPool();

	void Init(void);
	void DeInit(void);

	VkDescriptorPool GetDescriptorPool(void) const;
	VkDescriptorSetLayout GetDescriptorSetLayout(void) const;

protected:

	virtual void GetPoolSizes(std::vector<VkDescriptorPoolSize>& PoolSizes) = 0;
	virtual uint32 GetMaxSets(void) = 0;
	virtual void GetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& Bindings) = 0;

	VkDevice m_device;
	VkDescriptorPool m_descriptorPool;
	VkDescriptorSetLayout m_descriptorSetLayout;
};

