
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMesh/LeVkMesh/LeVkMesh.h"


LeVkMatBlock::LeVkMatBlock()
{
	
}


LeVkMatBlock::~LeVkMatBlock()
{
	if (m_vb.IsAlloced())
		LeVkUtils::DestroyBuffer(&m_vb);

	if (m_ib.IsAlloced())
		LeVkUtils::DestroyBuffer(&m_ib);
}



LeVkMesh::LeVkMesh()
{
	VkDevice Device = LeVkCore::Singleton().GetDevice();
	const LeVkPhysicalDevice* pPhysicalDevice = LeVkCore::Singleton().GetPhysicalDevice();

	LeVkUtils::CreateUniformBuffer(Device, &pPhysicalDevice->m_memProps, sizeof(glm::mat4), &m_modelMatrix);

	glm::mat4 GlobalMatrix = glm::mat4(1.0f);
	LeVkUtils::UpdateBuffer(&m_modelMatrix, &GlobalMatrix, sizeof(GlobalMatrix));

	m_descriptorSet.Init();
	m_descriptorSet.UpdateSet(&m_modelMatrix);
}

LeVkMesh::~LeVkMesh()
{
	LeVkUtils::DestroyBuffer(&m_modelMatrix);
}



static const VkVertexInputBindingDescription s_BindingDescription =
{
	0, sizeof(LeVertex), VK_VERTEX_INPUT_RATE_VERTEX
};

const VkVertexInputBindingDescription* LeVkMesh::LeVertexGetBindingDescription()
{
	return &s_BindingDescription;
}

static const VkVertexInputAttributeDescription s_AttributeDescriptions[] =
{
	{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LeVertex, pos) },
	{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LeVertex, color) },
	{ 2, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(LeVertex, uv) }
};

const VkVertexInputAttributeDescription* LeVkMesh::LeVertexGetAttributeDescriptions()
{
	return s_AttributeDescriptions;
}

uint32 LeVkMesh::LeVertexGetNumAttributeDescriptions()
{
	return ARRAY_SIZE(s_AttributeDescriptions);
}