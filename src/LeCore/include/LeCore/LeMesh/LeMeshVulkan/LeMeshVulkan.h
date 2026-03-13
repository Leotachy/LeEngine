
#pragma once

#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"

class LeMatBlockVk : public LeMatBlock
{
public:
	LeMatBlockVk();
	~LeMatBlockVk();

	LeVkGraphicsPipeline* GetGraphicsPipeline(void) { return m_pGraphicsPipeline; }
	void SetGraphicsPipeline(LeVkGraphicsPipeline* pGraphicsPipeline) { m_pGraphicsPipeline = pGraphicsPipeline; }

	LeVkMemoryBuffer* GetVertexBuffer(void) { return &m_vb; }

private:

	LeVkGraphicsPipeline* m_pGraphicsPipeline;

	LeVkMemoryBuffer m_vb;
};



const VkVertexInputBindingDescription* LeVertexGetBindingDescription();
const VkVertexInputAttributeDescription* LeVertexGetAttributeDescriptions();
uint32 LeVertexGetNumAttributeDescriptions();