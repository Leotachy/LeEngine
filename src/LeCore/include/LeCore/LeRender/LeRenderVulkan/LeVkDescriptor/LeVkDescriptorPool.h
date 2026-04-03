
#pragma once

// Base class for LeVkDescriptorPool

enum LeVkDescriptorPoolType
{
	GLOBAL,
	MATBLOCK,
	MATERIAL,

	NUM_DESCRIPTOR_POOLS
};

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

class LeVkDescriptorPoolManager
{
public:
	static LeVkDescriptorPoolManager& Singleton();

	void Init();
	void DeInit();

	LeVkDescriptorPool* GetLeDescriptorPool(LeVkDescriptorPoolType Type);

private:

	LeVkDescriptorPool* m_descriptorPools[LeVkDescriptorPoolType::NUM_DESCRIPTOR_POOLS];
};