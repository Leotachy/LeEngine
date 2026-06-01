
#pragma once

#include "LeCore/LeMesh/LeMesh.h"

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

	LeVkMesh(void);
	~LeVkMesh(void);

	LeVkAllocatedBuffer* GetModelMatrixUBO(void) { return &m_ModelMatrix; }

	VkDescriptorSet GetDescriptorSet(void) { return m_hDescriptorSet; }
	void SetDescriptorSet(VkDescriptorSet hDescriptorSet) { m_hDescriptorSet = hDescriptorSet; }

private:
	LeVkAllocatedBuffer m_ModelMatrix;
	VkDescriptorSet m_hDescriptorSet;
};
