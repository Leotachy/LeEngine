#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSet.h"

class LeVkDescriptorSetMesh : public LeVkDescriptorSet
{
public:

	LeVkDescriptorSetMesh() {};
	virtual ~LeVkDescriptorSetMesh() {};

	void UpdateSet(void* pData) override;
protected:
	LeVkDescriptorPoolType GetPoolType(void) override;
};