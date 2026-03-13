
#include "LeCore/LeCore.h"
#include "LeCore/LeMesh/LeMeshVulkan/LeMeshVulkan.h"


LeMatBlockVk::LeMatBlockVk()
{
	m_pGraphicsPipeline = NULL;
}


LeMatBlockVk::~LeMatBlockVk()
{

}

static const VkVertexInputBindingDescription s_BindingDescription =
{
	0, sizeof(LeVertex), VK_VERTEX_INPUT_RATE_VERTEX
};

const VkVertexInputBindingDescription* LeVertexGetBindingDescription()
{
	return &s_BindingDescription;
}

static const VkVertexInputAttributeDescription s_AttributeDescriptions[] =
{
	{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LeVertex, pos) },
	{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LeVertex, color) },
	{ 2, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(LeVertex, uv) }
};

const VkVertexInputAttributeDescription* LeVertexGetAttributeDescriptions()
{
	return s_AttributeDescriptions;
}

uint32 LeVertexGetNumAttributeDescriptions()
{
	return ARRAY_SIZE(s_AttributeDescriptions);
}