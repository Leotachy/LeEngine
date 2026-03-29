

#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMatBlock.h"


LeVkDescriptorSetMatBlock::LeVkDescriptorSetMatBlock(VkDescriptorPool DescriptorPool, uint32 Count) :
	LeVkDescriptorSet(DescriptorPool, Count)
{



}

LeVkDescriptorSetMatBlock::~LeVkDescriptorSetMatBlock()
{

}


void LeVkDescriptorSetMatBlock::UpdateSets(LeVkMemoryBuffer* ModelMatrice)
{

	LE_DEBUG_ASSERT(ModelMatrice);

	uint32 NumSets = uint32(m_descriptorSets.size());
	for (uint32 i = 0; i < NumSets; i++)
	{

		VkDescriptorBufferInfo BufferInfoUniform =
		{
			.buffer = ModelMatrice->buffer,
			.offset = 0,
			.range = VK_WHOLE_SIZE,
		};

		VkWriteDescriptorSet WriteDescriptorSet =
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = m_descriptorSets[i],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &BufferInfoUniform
		};

		vkUpdateDescriptorSets(m_device, 1, &WriteDescriptorSet, 0, NULL);
	}


}

void LeVkDescriptorSetMatBlock::GetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& Bindings)
{

	Bindings.emplace_back(VkDescriptorSetLayoutBinding{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		}
	);
}
