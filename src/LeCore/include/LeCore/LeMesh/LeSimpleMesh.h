
#pragma once

#include "LeCore/LeRender/LeRender.h"

struct LeVertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 uv;
};

struct LeSimpleMesh
{
	LeMemoryBuffer m_vb;
	size_t m_vertexBufferSize = 0;
	LeVulkanTexture* m_pTex = NULL;

	void Destroy(VkDevice Device)
	{
		m_vb.Destroy(Device);

		if (m_pTex)
		{
			m_pTex->Destroy(Device);
		}
	}

};