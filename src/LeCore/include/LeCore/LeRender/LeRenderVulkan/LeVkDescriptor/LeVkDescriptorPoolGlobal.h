#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"

class LeVkDescriptorPoolGlobal : public LeVkDescriptorPool
{

public:

	LeVkDescriptorPoolGlobal(uint32 DescriptorCount);
	~LeVkDescriptorPoolGlobal() {};



};