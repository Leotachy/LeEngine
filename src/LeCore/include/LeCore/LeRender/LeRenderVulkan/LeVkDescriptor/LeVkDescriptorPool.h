
#pragma once

// Base class for LeVkDescriptorPool

enum LeVkDescriptorPoolType
{
	NONE = 0,

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


	VkDescriptorPool GetDescriptorPool(void) const { return m_descriptorPool; }
protected:

	LeVkDescriptorPoolType m_type;
	VkDevice m_device;
	VkDescriptorPool m_descriptorPool;

};