
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeShader.h"
#include "LeCore/LeRender/LeGraphicsPipeline.h"

#include "LeRenderUtils.h"


LeGraphicsPipeline::LeGraphicsPipeline()
{
	m_pipeline = NULL;
	m_pipelineLayout = NULL;

	m_device = NULL;
}

LeGraphicsPipeline::~LeGraphicsPipeline()
{

}


void LeGraphicsPipeline::Init(VkDevice Device, const LeSwapchain* pSwapchain, LeShader* pShader)
{

	LE_DEBUG_ASSERT(pSwapchain);
	LE_DEBUG_ASSERT(pShader);

	m_device = Device;

	const VkPipelineShaderStageCreateInfo ShaderStageInfo[] =
	{
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = pShader->GetShaderModule(),
			.pName = "vertMain",
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = pShader->GetShaderModule(),
			.pName = "fragMain"
		}
	};
	const uint32 NumShaderStages = ARRAY_SIZE(ShaderStageInfo);




	// Dynamic parameters
	// These can be changed without needing to rebuild the graphics pipeline

	const VkDynamicState DynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	const uint32 NumDynamicStates = ARRAY_SIZE(DynamicStates);

	const VkPipelineDynamicStateCreateInfo DynamicStateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = NumDynamicStates,
		.pDynamicStates = DynamicStates
	};

	VkPipelineVertexInputStateCreateInfo VertexInputInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = LeVertexGetBindingDescription(),
		.vertexAttributeDescriptionCount = LeVertexGetNumAttributeDescriptions(),
		.pVertexAttributeDescriptions = LeVertexGetAttributeDescriptions()

	};

	VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};


	VkPipelineViewportStateCreateInfo ViewportStateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount = 1
	};

	VkPipelineRasterizationStateCreateInfo RasterizationStateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.lineWidth = 1.0f
	};

	VkPipelineMultisampleStateCreateInfo MultisampleStateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f
	};

	VkPipelineColorBlendAttachmentState BlendAttachState =
	{
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineColorBlendStateCreateInfo BlendStateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &BlendAttachState
	};

	VkPipelineLayoutCreateInfo LayoutInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = NULL
	};

	VkResult res = vkCreatePipelineLayout(Device, &LayoutInfo, NULL, &m_pipelineLayout);
	LE_CHECK_VK_RESULT(res, "vkCreatePipelineLayout");

	// Dynamic rendering config

	VkFormat ColorAttachmentFormat = pSwapchain->GetImageFormat();

	VkPipelineRenderingCreateInfo RenderingInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &ColorAttachmentFormat,
	};

	VkGraphicsPipelineCreateInfo PipelineInfo =
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &RenderingInfo,
		.stageCount = NumShaderStages,
		.pStages = ShaderStageInfo,
		.pVertexInputState = &VertexInputInfo,
		.pInputAssemblyState = &InputAssemblyInfo,
		.pViewportState = &ViewportStateInfo,
		.pRasterizationState = &RasterizationStateInfo,
		.pMultisampleState = &MultisampleStateInfo,
		.pColorBlendState = &BlendStateInfo,
		.pDynamicState = &DynamicStateInfo,
		.layout = m_pipelineLayout,
		.renderPass = NULL,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	res = vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &PipelineInfo, NULL, &m_pipeline);
	LE_CHECK_VK_RESULT(res, "vkCreateGraphicsPipelines");

	LE_DEBUG_MESSAGE("Graphics pipeline created");
}

void LeGraphicsPipeline::Destroy(void)
{
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, NULL);
	vkDestroyPipeline(m_device, m_pipeline, NULL);

	LE_DEBUG_MESSAGE("Graphics pipeline destroyed");
}

void LeGraphicsPipeline::Bind(VkCommandBuffer CmdBuf)
{
	vkCmdBindPipeline(CmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
}

