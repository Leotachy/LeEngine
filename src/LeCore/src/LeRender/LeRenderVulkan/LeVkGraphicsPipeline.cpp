
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptorSet.h"
#include "LeCore/LeMesh/LeMeshVulkan/LeMeshVulkan.h"

LeVkGraphicsPipeline::LeVkGraphicsPipeline(uint64 key, int32 NumImages, LeShaderVk* pShader, VkFormat RenderColFormat)
{
	LE_DEBUG_ASSERT(key);
	LE_DEBUG_ASSERT(NumImages > 0);
	LE_DEBUG_ASSERT(pShader);

	m_key = key;

	m_device = LeGetVkLogicalDevice();

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

	VkPipelineLayoutCreateInfo LayoutInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = (uint32)LeVkGetNumDescriptorSetLayouts(),
		.pSetLayouts = LeVkGetDescriptorSetLayouts()
	};



	VkResult res = vkCreatePipelineLayout(m_device, &LayoutInfo, NULL, &m_pipelineLayout);
	LE_CHECK_VK_RESULT(res, "vkCreatePipelineLayout");

	// Dynamic rendering config

	VkFormat ColorAttachmentFormat = RenderColFormat;

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

	res = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &PipelineInfo, NULL, &m_pipeline);
	LE_CHECK_VK_RESULT(res, "vkCreateGraphicsPipelines");

	LE_DEBUG_MESSAGE("Graphics pipeline created");


}

LeVkGraphicsPipeline::~LeVkGraphicsPipeline()
{
	if (m_device)
	{
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, NULL);
		vkDestroyPipeline(m_device, m_pipeline, NULL);
	}

	LE_DEBUG_MESSAGE("Graphics pipeline destroyed");
}




LeVkGraphicsPipelineDictionary* LeVkGraphicsPipelineDictionary::Singleton(void)
{
	static LeVkGraphicsPipelineDictionary instance;
	return &instance;
}

void LeVkGraphicsPipelineDictionary::Add(LeVkGraphicsPipeline* pPipeline)
{
	uint64 key = pPipeline->GetKey();

	if (m_GraphicsPipelines[key])
		LE_WARN("Pipeline already exists with key %lld", key);

	m_GraphicsPipelines[key] = pPipeline;
}

LeVkGraphicsPipeline* LeVkGraphicsPipelineDictionary::Find(int64 key)
{
	return m_GraphicsPipelines[key];
}

void LeVkGraphicsPipelineDictionary::Purge(void)
{
	for (auto pipeline : m_GraphicsPipelines)
		delete pipeline.second;

	m_GraphicsPipelines.clear();
}