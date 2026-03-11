
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"

LeVkMemoryBuffer::LeVkMemoryBuffer()
{

	device = VK_NULL_HANDLE;

	mem = VK_NULL_HANDLE;
	buffer = VK_NULL_HANDLE;
	allocationSize = 0;
}

LeVkMemoryBuffer::~LeVkMemoryBuffer()
{
	if (device)
	{
		if (mem)
			vkFreeMemory(device, mem, NULL);

		if (buffer)
			vkDestroyBuffer(device, buffer, NULL);
	}
}

void LeVkMemoryBuffer::Update(const void* pData, size_t Size)
{
	void* pMem = NULL;
	VkResult res = vkMapMemory(device, mem, 0, Size, 0, &pMem);
	LE_CHECK_VK_RESULT(res, "vkMapMemory");
	memcpy(pMem, pData, Size);
	vkUnmapMemory(device, mem);
}