#include "LeCore/lecore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMaterial.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"

void LeVkDescriptorSetMaterial::UpdateSet(void* pData)
{

	LeVkTexture* Texture = (LeVkTexture*)pData;
	LE_DEBUG_ASSERT(Texture);

	VkDescriptorImageInfo DescriptorImageInfo =
	{
		.sampler = Texture->sampler,
		.imageView = Texture->allocatedImage.imageView,
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
	};

	VkWriteDescriptorSet WriteDescriptorSet =
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = m_descriptorSet,
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.pImageInfo = &DescriptorImageInfo
	};

	vkUpdateDescriptorSets(m_device, 1, &WriteDescriptorSet, 0, NULL);
}

LeVkDescriptorPoolType LeVkDescriptorSetMaterial::GetPoolType(void)
{
	return LeVkDescriptorPoolType::MATERIAL;
}