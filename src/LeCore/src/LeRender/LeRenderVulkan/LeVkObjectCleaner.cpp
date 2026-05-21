
#include "LeCore/LeCore.h"

#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkObjectCleaner.h"

void LeVkObjectCleaner::Init(void)
{

}

void LeVkObjectCleaner::DeInit(void)
{
	for (uint32 i = 0; i < LE_MAX_FRAMES_IN_FLIGHT; i++)
		Flush(i);
}

void LeVkObjectCleaner::PushBuffer(LeVkAllocatedBuffer* pAllocatedBuffer, uint32 FrameIndex)
{
	LE_DEBUG_ASSERT(FrameIndex < LE_MAX_FRAMES_IN_FLIGHT);

	m_buffers[FrameIndex].emplace_back(*pAllocatedBuffer);

}

void LeVkObjectCleaner::Flush(uint32 FrameIndex)
{
	LE_DEBUG_ASSERT(FrameIndex < LE_MAX_FRAMES_IN_FLIGHT);

	for (uint32 i = 0; i < m_buffers[FrameIndex].size(); i++)
		LeVkUtils::DestroyBuffer(&m_buffers[FrameIndex].at(i));

	m_buffers[FrameIndex].clear();
}