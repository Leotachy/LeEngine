#pragma once


#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSet.h"

class LeVkDescriptorSetGlobal : public LeVkDescriptorSet
{

public:
	LeVkDescriptorSetGlobal() {};
	virtual ~LeVkDescriptorSetGlobal() {};

	void UpdateSet(void* pData) override;

protected:
	LeVkDescriptorPoolType GetPoolType(void) override;
};