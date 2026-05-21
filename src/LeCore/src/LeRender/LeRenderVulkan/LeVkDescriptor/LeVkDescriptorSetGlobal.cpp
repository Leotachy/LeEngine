
#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetGlobal.h"


void LeVkDescriptorSetGlobal::UpdateSet(void* pData)
{
	LeVkAllocatedBuffer* WpMatrix = (LeVkAllocatedBuffer*)pData;
	LE_DEBUG_ASSERT(WpMatrix);

	VkDescriptorBufferInfo BufferInfoUniform =
	{
		.buffer = WpMatrix->buffer,
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

LeVkDescriptorPoolType LeVkDescriptorSetGlobal::GetPoolType(void)
{
	return LeVkDescriptorPoolType::GLOBAL;
}