
#pragma once

#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMatBlock.h"

class LeMatBlockVk : public LeMatBlock
{
public:
	LeMatBlockVk();
	~LeMatBlockVk();

	LeVkMemoryBuffer* GetVertexBuffer(void) { return &m_vb; }
	LeVkMemoryBuffer* GetIndexBuffer(void) { return &m_ib; }

private:


	LeVkMemoryBuffer m_vb;
	LeVkMemoryBuffer m_ib;

};



const VkVertexInputBindingDescription* LeVertexGetBindingDescription();
const VkVertexInputAttributeDescription* LeVertexGetAttributeDescriptions();
uint32 LeVertexGetNumAttributeDescriptions();