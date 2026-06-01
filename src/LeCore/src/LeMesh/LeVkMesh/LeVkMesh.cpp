
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMesh/LeVkMesh/LeVkMesh.h"


LeVkMatBlock::LeVkMatBlock()
{
	m_VertexBuffer = {};
	m_IndexBuffer = {};
}


LeVkMatBlock::~LeVkMatBlock()
{
	if (m_bIsCompiled)
	{
		LeVkUtils::DestroyBuffer(&m_VertexBuffer);
		LeVkUtils::DestroyBuffer(&m_IndexBuffer);
	}
}

void LeVkMatBlock::Compile(void)
{

	if (m_bIsCompiled)
	{
		// Cannot compile again
		return;
	}

	{
		LE_ASSERT(m_Vertices.size() > 0 && "Compiling matblock with 0 vertices!");

		VkBufferUsageFlags Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VkDeviceSize VertexBufferSize = sizeof(LeVertex) * m_Vertices.size();
		VmaAllocationCreateFlags AllocCreateFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

		LeVkUtils::CreateBuffer(VertexBufferSize, Usage, AllocCreateFlags, &m_VertexBuffer);
	}

	{
		LE_ASSERT(m_Faces.size() > 0 && "Compiling matblock with 0 faces!");

		VkBufferUsageFlags Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VkDeviceSize IndexBufferSize = sizeof(LeFace) * m_Faces.size();
		VmaAllocationCreateFlags AllocCreateFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

		LeVkUtils::CreateBuffer(IndexBufferSize, Usage, AllocCreateFlags, &m_IndexBuffer);
	}


	m_bIsCompiled = true;
}



static const VkVertexInputBindingDescription s_BindingDescription =
{
	0, sizeof(LeVertex), VK_VERTEX_INPUT_RATE_VERTEX
};

const VkVertexInputBindingDescription* LeVkMatBlock::LeVertexGetBindingDescription()
{
	return &s_BindingDescription;
}

static const VkVertexInputAttributeDescription s_AttributeDescriptions[] =
{
	{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LeVertex, pos) },
	{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LeVertex, color) },
	{ 2, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(LeVertex, uv) }
};

const VkVertexInputAttributeDescription* LeVkMatBlock::LeVertexGetAttributeDescriptions()
{
	return s_AttributeDescriptions;
}

uint32 LeVkMatBlock::LeVertexGetNumAttributeDescriptions()
{
	return ARRAY_SIZE(s_AttributeDescriptions);
}


LeVkMesh::LeVkMesh()
{
	LeVkUtils::CreateUniformBuffer(sizeof(glm::mat4), &m_ModelMatrix);

	glm::mat4 GlobalMatrix = glm::mat4(1.0f);
	LeVkUtils::UpdateBuffer(&m_ModelMatrix, &GlobalMatrix, sizeof(GlobalMatrix));

	m_hDescriptorSet = VK_NULL_HANDLE;
}

LeVkMesh::~LeVkMesh()
{
	LeVkUtils::DestroyBuffer(&m_ModelMatrix);
}

