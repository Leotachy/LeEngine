#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryImage.h"


LeBool LeVkMemoryImage::IsAlloced(void) const
{
	return image != VK_NULL_HANDLE;
}