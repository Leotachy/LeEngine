
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptorSet.h"

static VkDevice s_Device = VK_NULL_HANDLE;

static VkDescriptorPool s_DescriptorPool = VK_NULL_HANDLE;
static VkDescriptorSetLayout s_descriptorSetLayout = VK_NULL_HANDLE;
static std::vector<VkDescriptorSet> s_descriptorSets;

static void createDescriptorPool()
{
	VkDescriptorPoolSize PoolSizes[] =
	{
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = (uint32)LE_MAX_FRAMES_IN_FLIGHT
		}
	};

	VkDescriptorPoolCreateInfo PoolInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = 0,
		.maxSets = (uint32)LE_MAX_FRAMES_IN_FLIGHT,
		.poolSizeCount = ARRAY_SIZE(PoolSizes),
		.pPoolSizes = PoolSizes
	};


	VkResult res = vkCreateDescriptorPool(s_Device, &PoolInfo, NULL, &s_DescriptorPool);
	LE_CHECK_VK_RESULT(res, "vkCreateDescriptorPool");
}


void createDescriptorSetLayout()
{
	std::vector<VkDescriptorSetLayoutBinding> LayoutBindings;

	// frame global stuff
	LayoutBindings.emplace_back(VkDescriptorSetLayoutBinding
		{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		});


	VkDescriptorSetLayoutCreateInfo LayoutInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,			// reserved - must be zero
		.bindingCount = (uint32)LayoutBindings.size(),
		.pBindings = LayoutBindings.data()
	};

	VkResult res = vkCreateDescriptorSetLayout(s_Device, &LayoutInfo, NULL, &s_descriptorSetLayout);
	LE_CHECK_VK_RESULT(res, "vkCreateDescriptorSetLayout");
}


static void allocateDescriptorSets(void)
{
	std::vector<VkDescriptorSetLayout> Layouts(LE_MAX_FRAMES_IN_FLIGHT, s_descriptorSetLayout);

	VkDescriptorSetAllocateInfo AllocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = s_DescriptorPool,
		.descriptorSetCount = (uint32)LE_MAX_FRAMES_IN_FLIGHT,
		.pSetLayouts = Layouts.data()
	};

	s_descriptorSets.resize(LE_MAX_FRAMES_IN_FLIGHT);

	VkResult res = vkAllocateDescriptorSets(s_Device, &AllocInfo, s_descriptorSets.data());
	LE_CHECK_VK_RESULT(res, "vkAllocateDescriptorSets");
}


static void linkDescriptorSets(LeVkMemoryBuffer* pMemoryBuffer, int32 FrameIndex)
{

	LE_DEBUG_ASSERT(pMemoryBuffer);
	LE_DEBUG_ASSERT(FrameIndex >= 0 && FrameIndex < s_descriptorSets.size());

	VkDescriptorBufferInfo BufferInfoUniform =
	{
		.buffer = pMemoryBuffer->buffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE,
	};

	VkWriteDescriptorSet WriteDescriptorSet = 
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = s_descriptorSets[FrameIndex],
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.pBufferInfo = &BufferInfoUniform
	};

	vkUpdateDescriptorSets(s_Device, 1, &WriteDescriptorSet, 0, NULL);
}


void LeVkInitDescriptorSets(VkDevice Device)
{
	s_Device = Device;

	createDescriptorPool();
	createDescriptorSetLayout();
	allocateDescriptorSets();
}

void LeVkLinkDescriptorSets(LeVkMemoryBuffer* pMemoryBuffer, int32 FrameIndex)
{
	linkDescriptorSets(pMemoryBuffer, FrameIndex);
}

VkDescriptorSet* LeVkGetDescriptorSet(int32 FrameIndex)
{
	return &s_descriptorSets[FrameIndex];
}

void LeVkDeInitDescriptorSets(void)
{
	if (s_Device)
	{
		vkDestroyDescriptorSetLayout(s_Device, s_descriptorSetLayout, NULL);
		vkDestroyDescriptorPool(s_Device, s_DescriptorPool, NULL);
	}

}

int32 LeVkGetNumDescriptorSetLayouts(void)
{
	return 1;
}

VkDescriptorSetLayout* LeVkGetDescriptorSetLayouts(void)
{
	return &s_descriptorSetLayout;
}

