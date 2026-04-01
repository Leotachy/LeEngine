
#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMesh.h"

void LeVkDescriptorSetMesh::UpdateSet(void* pData)
{
	LeVkMemoryBuffer* ModelMatrix = (LeVkMemoryBuffer*)pData;
	LE_DEBUG_ASSERT(ModelMatrix);

	VkDescriptorBufferInfo BufferInfoUniform =
	{
		.buffer = ModelMatrix->buffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE,
	};

	VkWriteDescriptorSet WriteDescriptorSet =
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = m_descriptorSet,
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.pBufferInfo = &BufferInfoUniform
	};

	vkUpdateDescriptorSets(m_device, 1, &WriteDescriptorSet, 0, NULL);
}

LeVkDescriptorPoolType LeVkDescriptorSetMesh::GetPoolType(void)
{
	return LeVkDescriptorPoolType::MATBLOCK;
}