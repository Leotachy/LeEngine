
#pragma once

#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMesh.h"

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


class LeMeshVk : public LeMesh
{

public:

	LeMeshVk();
	~LeMeshVk();

	LeVkDescriptorSetMesh* GetLeDescriptor(void) { return &m_descriptorSet; }

	static const VkVertexInputBindingDescription* LeVertexGetBindingDescription();
	static const VkVertexInputAttributeDescription* LeVertexGetAttributeDescriptions();
	static uint32 LeVertexGetNumAttributeDescriptions();

protected:

	LeVkMemoryBuffer m_modelMatrix;

	LeVkDescriptorSetMesh m_descriptorSet;
};
