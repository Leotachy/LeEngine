
#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"

void LeVkInitDescriptorSets(VkDevice Device);
void LeVkLinkDescriptorSets(LeVkMemoryBuffer* pMemoryBuffer, int32 FrameIndex);
VkDescriptorSet* LeVkGetDescriptorSet(int32 FrameIndex);
void LeVkDeInitDescriptorSets(void);

int32 LeVkGetNumDescriptorSetLayouts(void);
VkDescriptorSetLayout* LeVkGetDescriptorSetLayouts(void);