
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

	virtual void GetPoolSizes(VkDescriptorPoolSize** PoolSizes, uint32* NumSizes) = 0;
	virtual uint32 GetMaxSets(void) = 0;
	virtual void GetLayoutBindings(VkDescriptorSetLayoutBinding** Bindings, uint32* NumSizes) = 0;

	VkDevice m_hDevice;
	VkDescriptorPool m_descriptorPool;
	VkDescriptorSetLayout m_descriptorSetLayout;
};

