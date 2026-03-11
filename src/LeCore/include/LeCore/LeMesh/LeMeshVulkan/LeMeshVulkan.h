
#pragma once

#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"

class LeMatblockVk : public LeMatBlock
{
public:
	LeMatblockVk();
	~LeMatblockVk();

private:

	LeVkMemoryBuffer m_vb;
	size_t m_vertexBufferSize = 0;

};