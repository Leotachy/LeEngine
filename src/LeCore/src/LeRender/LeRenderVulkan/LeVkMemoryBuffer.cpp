
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"

LeBool LeVkMemoryBuffer::IsAlloced(void) const
{
	return buffer != VK_NULL_HANDLE;
}