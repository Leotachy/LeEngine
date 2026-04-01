#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSet.h"

class LeVkDescriptorSetMaterial : public LeVkDescriptorSet
{

public:

	LeVkDescriptorSetMaterial() {};
	virtual ~LeVkDescriptorSetMaterial() {};

	void UpdateSet(void* pData) override;
protected:
	LeVkDescriptorPoolType GetPoolType(void) override;
};