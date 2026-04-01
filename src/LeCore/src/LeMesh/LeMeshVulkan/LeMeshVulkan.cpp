
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMesh/LeMeshVulkan/LeMeshVulkan.h"


LeMatBlockVk::LeMatBlockVk()
{
	
}


LeMatBlockVk::~LeMatBlockVk()
{
}

static const VkVertexInputBindingDescription s_BindingDescription =
{
	0, sizeof(LeVertex), VK_VERTEX_INPUT_RATE_VERTEX
};

const VkVertexInputBindingDescription* LeMatBlockVk::LeVertexGetBindingDescription()
{
	return &s_BindingDescription;
}

static const VkVertexInputAttributeDescription s_AttributeDescriptions[] =
{
	{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LeVertex, pos) },
	{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LeVertex, color) },
	{ 2, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(LeVertex, uv) }
};

const VkVertexInputAttributeDescription* LeMatBlockVk::LeVertexGetAttributeDescriptions()
{
	return s_AttributeDescriptions;
}

uint32 LeMatBlockVk::LeVertexGetNumAttributeDescriptions()
{
	return ARRAY_SIZE(s_AttributeDescriptions);
}


LeMeshVk::LeMeshVk()
{
	VkDevice Device = LeVkCore::Singleton().LeVkGetLogicalDevice();
	const LeVkPhysicalDevice* pPhysicalDevice = LeVkCore::Singleton().LeVkGetPhysicalDevice();

	LeVkUtils::CreateUniformBuffer(Device, &pPhysicalDevice->m_memProps, sizeof(glm::mat4), &m_modelMatrix);

	glm::mat4 GlobalMatrix = glm::mat4(1.0f);
	LeVkUtils::UpdateBuffer(&m_modelMatrix, &GlobalMatrix, sizeof(GlobalMatrix));

	m_descriptorSet.UpdateSet(&m_modelMatrix);
}

LeMeshVk::~LeMeshVk()
{
	LeVkUtils::DestroyBuffer(&m_modelMatrix);
}