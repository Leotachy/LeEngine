
#pragma once

#include "LeCore/LeRender/LeRender.h"

struct LeSimpleMesh
{
	LeMemoryBuffer m_vb;
	size_t m_vertexBufferSize = 0;

	void Destroy(VkDevice Device)
	{
		m_vb.Destroy(Device);
	}

};