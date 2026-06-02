
#include "LeCore/LeCore.h"

#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"

#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor.h"


void LeVkDescriptor::Init(const VkDescriptorType* pDescriptorTypes, uint32 NumTypes, VkShaderStageFlags ShaderStageFlags, uint32 MaxSets)
{

	const uint32 SET_LAYOUT_BINDINGS_BUFFER_SIZE = 50;
	LE_DEBUG_ASSERT(NumTypes < SET_LAYOUT_BINDINGS_BUFFER_SIZE);

	VkDevice device = LeVkRenderer::Get()->GetDevice();

	VkResult res;

	// Create set layout

	VkDescriptorSetLayoutBinding SetLayoutBindings[SET_LAYOUT_BINDINGS_BUFFER_SIZE];

	for (uint32 i = 0; i < NumTypes; i++)
	{
		SetLayoutBindings[i] =
		{
			.binding = i,
			.descriptorType = pDescriptorTypes[i],
			.descriptorCount = 1,
			.stageFlags = ShaderStageFlags
		};
	}

	VkDescriptorSetLayoutCreateInfo SetLayoutCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = NumTypes,
		.pBindings = SetLayoutBindings
	};

	res = vkCreateDescriptorSetLayout(device, &SetLayoutCreateInfo, NULL, &m_hLayout);
	LE_CHECK_VK_RESULT(res);
	

	// Create pools

	const uint32 POOL_SIZES_BUFFER_SIZE = SET_LAYOUT_BINDINGS_BUFFER_SIZE;
	LE_DEBUG_ASSERT(NumTypes < POOL_SIZES_BUFFER_SIZE);

	for (uint32 i = 0; i < LE_MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorPoolSize PoolSizes[POOL_SIZES_BUFFER_SIZE];

		for (uint32 i = 0; i < NumTypes; i++)
		{
			PoolSizes[i] =
			{
				.type = pDescriptorTypes[i],
				.descriptorCount = 1 * MaxSets
			};
		}

		VkDescriptorPoolCreateInfo PoolCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = MaxSets,
			.poolSizeCount = NumTypes,
			.pPoolSizes = PoolSizes
		};

		VkResult res = vkCreateDescriptorPool(device, &PoolCreateInfo, NULL, &m_Pools[i].hPool);
		LE_CHECK_VK_RESULT(res);

		m_Pools[i].NumSetsActive = 0;
	}

	m_MaxSetsPerPool = MaxSets;
}

void LeVkDescriptor::DeInit(void)
{
	VkDevice device = LeVkRenderer::Get()->GetDevice();

	vkDestroyDescriptorSetLayout(device, m_hLayout, NULL);
	m_hLayout = NULL;

	m_MaxSetsPerPool = 0;

	for (uint32 i = 0; i < LE_MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyDescriptorPool(device, m_Pools[i].hPool, NULL);
		m_Pools[i].hPool = NULL;

		m_Pools[i].NumSetsActive = 0;
	}
}

void LeVkDescriptor::Reset(uint32 FrameIndex)
{
	LE_DEBUG_ASSERT(FrameIndex < LE_MAX_FRAMES_IN_FLIGHT);

	VkDevice device = LeVkRenderer::Get()->GetDevice();

	vkResetDescriptorPool(device, m_Pools[FrameIndex].hPool, 0);
}

VkDescriptorSetLayout LeVkDescriptor::GetDescriptorSetLayout(void)
{
	return m_hLayout;
}

VkDescriptorSet LeVkDescriptor::CreateDescriptorSet(uint32 FrameIndex, void* pData)
{
	LE_DEBUG_ASSERT(FrameIndex < LE_MAX_FRAMES_IN_FLIGHT);
	LE_DEBUG_ASSERT(pData);

	VkDescriptorSetAllocateInfo AllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = m_Pools[FrameIndex].hPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &m_hLayout
	};


	VkDevice device = LeVkRenderer::Get()->GetDevice();

	VkDescriptorSet descriptorSet;
	VkResult res = vkAllocateDescriptorSets(device, &AllocInfo, &descriptorSet);
	LE_CHECK_VK_RESULT(res);

	return descriptorSet;
}

VkDescriptorSet LeVkFrameDescriptor::CreateDescriptorSet(uint32 FrameIndex, void* pData)
{

	VkDescriptorSet hDescriptorSet = LeVkDescriptor::CreateDescriptorSet(FrameIndex, pData);

	LeVkAllocatedBuffer* pBuffer = (LeVkAllocatedBuffer*)pData;

	VkDescriptorBufferInfo DescriptorBufferInfo =
	{
		.buffer = pBuffer->buffer,
		.offset = 0,
		.range = pBuffer->allocationInfo.size,
	};

	VkWriteDescriptorSet WriteDescriptorSet =
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = hDescriptorSet,
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.pBufferInfo = &DescriptorBufferInfo
	};


	VkDevice hDevice = LeVkRenderer::Get()->GetDevice();
	vkUpdateDescriptorSets(hDevice, 1, &WriteDescriptorSet, 0, NULL);

	return hDescriptorSet;
}
//
//VkDescriptorSet LeVkMeshDescriptor::CreateDescriptorSet(uint32 FrameIndex, void* pData)
//{
//	VkDescriptorSet hDescriptorSet = LeVkDescriptor::CreateDescriptorSet(FrameIndex, pData);
//
//	LeVkAllocatedBuffer* pBuffer = (LeVkAllocatedBuffer*)pData;
//
//	VkDescriptorBufferInfo DescriptorBufferInfo =
//	{
//		.buffer = pBuffer->buffer,
//		.offset = 0,
//		.range = pBuffer->allocationInfo.size,
//	};
//
//	VkWriteDescriptorSet WriteDescriptorSet =
//	{
//		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
//		.dstSet = hDescriptorSet,
//		.dstBinding = 0,
//		.dstArrayElement = 0,
//		.descriptorCount = 1,
//		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//		.pBufferInfo = &DescriptorBufferInfo
//	};
//
//
//	VkDevice hDevice = LeVkRenderer::Get()->GetDevice();
//	vkUpdateDescriptorSets(hDevice, 1, &WriteDescriptorSet, 0, NULL);
//
//	return hDescriptorSet;
//}

VkDescriptorSet LeVkMaterialDescriptor::CreateDescriptorSet(uint32 FrameIndex, void* pData)
{
	VkDescriptorSet hDescriptorSet = LeVkDescriptor::CreateDescriptorSet(FrameIndex, pData);

	MaterialDescriptorSetParams* pMaterialData = (MaterialDescriptorSetParams*)pData;

	VkDescriptorImageInfo DescriptorImageInfo =
	{
		.sampler = pMaterialData->BaseTextureInfo.hSampler,
		.imageView = pMaterialData->BaseTextureInfo.hImageView,
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
	};

	VkWriteDescriptorSet WriteDescriptorSet =
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = hDescriptorSet,
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.pImageInfo = &DescriptorImageInfo
	};


	VkDevice hDevice = LeVkRenderer::Get()->GetDevice();
	vkUpdateDescriptorSets(hDevice, 1, &WriteDescriptorSet, 0, NULL);

	return hDescriptorSet;
}

void LeVkDescriptorManager::Init(void)
{

	VkDescriptorType FrameDataTypes[] =
	{
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	};
	m_FrameData.Init(FrameDataTypes, ARRAY_SIZE(FrameDataTypes), VK_SHADER_STAGE_VERTEX_BIT, 1);


	/*VkDescriptorType MeshDataTypes[] =
	{
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	};
	m_MeshData.Init(MeshDataTypes, ARRAY_SIZE(MeshDataTypes), VK_SHADER_STAGE_VERTEX_BIT, 50000);*/


	VkDescriptorType MaterialDataTypes[] =
	{
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	};
	m_MaterialData.Init(MaterialDataTypes, ARRAY_SIZE(MaterialDataTypes), VK_SHADER_STAGE_FRAGMENT_BIT, 5000);

}


void LeVkDescriptorManager::DeInit(void)
{

	m_FrameData.DeInit();
	//m_MeshData.DeInit();
	m_MaterialData.DeInit();

}

void LeVkDescriptorManager::ResetDescriptors(uint32 FrameIndex)
{
	m_FrameData.Reset(FrameIndex);
	//m_MeshData.Reset(FrameIndex);
	m_MaterialData.Reset(FrameIndex);
}

LeVkDescriptor* LeVkDescriptorManager::GetDescriptor(LeVkDescriptorType type)
{
	switch (type)
	{
	case LeVkDescriptorType::Frame:
		return &m_FrameData;
	//case LeVkDescriptorType::Mesh:
	//	return &m_MeshData;
	case LeVkDescriptorType::Material:
		return &m_MaterialData;
	}

	LE_DEBUG_ERROR("Unknown descriptor type");
	return NULL;
}