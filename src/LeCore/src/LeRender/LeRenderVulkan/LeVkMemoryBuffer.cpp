
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"

LeVkMemoryBuffer::LeVkMemoryBuffer()
{

	device = VK_NULL_HANDLE;

	buffer = VK_NULL_HANDLE;
	mem = VK_NULL_HANDLE;
	allocationSize = 0;
}

LeVkMemoryBuffer::~LeVkMemoryBuffer()
{
	DeAlloc();
}

void LeVkMemoryBuffer::DeAlloc(void)
{
	if (Alloced())
	{
		if (buffer)
		{
			vkDestroyBuffer(device, buffer, NULL);
			buffer = VK_NULL_HANDLE;
		}

		if (mem)
		{
			vkFreeMemory(device, mem, NULL);
			mem = VK_NULL_HANDLE;
		}
			
		allocationSize = 0;

		device = VK_NULL_HANDLE;
	}
}

void LeVkMemoryBuffer::Update(const void* pData, size_t Size)
{
	void* pMem = NULL;
	VkResult res = vkMapMemory(device, mem, 0, Size, 0, &pMem);
	LE_CHECK_VK_RESULT(res, "vkMapMemory");
	LeMemcpy(pMem, pData, Size);
	vkUnmapMemory(device, mem);
}