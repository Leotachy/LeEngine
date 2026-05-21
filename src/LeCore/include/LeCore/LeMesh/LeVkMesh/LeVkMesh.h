
#pragma once

#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMesh.h"

class LeVkMatBlock : public LeMatBlock
{
public:
	LeVkMatBlock();
	~LeVkMatBlock();

	LeVkAllocatedBuffer* GetVertexBuffer(void) { return &m_VertexBuffer; }
	LeVkAllocatedBuffer* GetIndexBuffer(void) { return &m_IndexBuffer; }

	void Compile(void) override;


	static const VkVertexInputBindingDescription* LeVertexGetBindingDescription();
	static const VkVertexInputAttributeDescription* LeVertexGetAttributeDescriptions();
	static uint32 LeVertexGetNumAttributeDescriptions();

private:


	LeVkAllocatedBuffer m_VertexBuffer;
	LeVkAllocatedBuffer m_IndexBuffer;

};


class LeVkMesh : public LeMesh
{

public:

	LeVkMesh();
	~LeVkMesh();

	LeVkDescriptorSetMesh* GetLeDescriptor(void) { return &m_descriptorSet; }

protected:

	LeVkAllocatedBuffer m_modelMatrix;

	LeVkDescriptorSetMesh m_descriptorSet;
};
