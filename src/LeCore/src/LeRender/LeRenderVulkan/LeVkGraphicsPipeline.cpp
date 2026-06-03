
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"
#include "LeCore/LeMesh/LeVkMesh/LeVkMesh.h"

LeVkGraphicsPipeline::LeVkGraphicsPipeline(const LeVkGraphicsPipelineParams* pParams)
{
	LE_DEBUG_ASSERT(pParams);
	LE_DEBUG_ASSERT(pParams->pVertexShader);
	LE_DEBUG_ASSERT(pParams->pFragmentShader);

	const VkPipelineShaderStageCreateInfo ShaderStageInfo[] =
	{
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = pParams->pVertexShader->GetShaderModule(),
			.pName = "main"
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = pParams->pFragmentShader->GetShaderModule(),
			.pName = "main"
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
		.pVertexBindingDescriptions = LeVkMatBlock::LeVertexGetBindingDescription(),
		.vertexAttributeDescriptionCount = LeVkMatBlock::LeVertexGetNumAttributeDescriptions(),
		.pVertexAttributeDescriptions = LeVkMatBlock::LeVertexGetAttributeDescriptions()

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
		.cullMode = VkCullModeFlags(pParams->bDoubleSided ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT),
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

	VkPipelineDepthStencilStateCreateInfo DepthStencilStateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
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

	VkDescriptorSetLayout PipelineLayouts[uint64(LeVkDescriptorType::NumDescriptorTypes)];
	LeVkDescriptorManager* pDescriptorManager = LeVkRenderer::Get()->GetDescriptorManager();
	LE_DEBUG_ASSERT(pDescriptorManager);

	for (uint32 i = 0; i < uint32(LeVkDescriptorType::NumDescriptorTypes); i++)
	{
		LeVkDescriptor* pDescriptor = pDescriptorManager->GetDescriptor(LeVkDescriptorType(i));
		LE_DEBUG_ASSERT(pDescriptor);

		PipelineLayouts[i] = pDescriptor->GetDescriptorSetLayout();
	}

	VkPushConstantRange PushConstant =
	{
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof(LeVkPushConstants)
	};
	

	VkPipelineLayoutCreateInfo LayoutInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = uint32(LeVkDescriptorType::NumDescriptorTypes),
		.pSetLayouts = PipelineLayouts,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &PushConstant
	};



	VkResult res = vkCreatePipelineLayout(LeVkRenderer::Get()->GetDevice(), &LayoutInfo, NULL, &m_hPipelineLayout);
	LE_CHECK_VK_RESULT(res);

	// Dynamic rendering config

	VkPipelineRenderingCreateInfo RenderingInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &LeVkRenderer::VIEWPORT_COLOR_IMAGE_FORMAT,
		.depthAttachmentFormat = LeVkRenderer::VIEWPORT_DEPTH_IMAGE_FORMAT
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
		.pDepthStencilState = &DepthStencilStateInfo,
		.pColorBlendState = &BlendStateInfo,
		.pDynamicState = &DynamicStateInfo,
		.layout = m_hPipelineLayout,
		.renderPass = NULL,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	res = vkCreateGraphicsPipelines(LeVkRenderer::Get()->GetDevice(), VK_NULL_HANDLE, 1, &PipelineInfo, NULL, &m_hPipeline);
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Graphics pipeline created");


}

LeVkGraphicsPipeline::~LeVkGraphicsPipeline()
{
	vkDestroyPipelineLayout(LeVkRenderer::Get()->GetDevice(), m_hPipelineLayout, NULL);
	vkDestroyPipeline(LeVkRenderer::Get()->GetDevice(), m_hPipeline, NULL);

	LE_DEBUG_MESSAGE("Graphics pipeline destroyed");
}




void LeVkGraphicsPipelineDictionary::Init()
{

}

void LeVkGraphicsPipelineDictionary::DeInit()
{
	PurgeAllGraphicsPipelines();
}

const LeVkGraphicsPipeline* LeVkGraphicsPipelineDictionary::GetOrCreateGraphicsPipeline(const LeVkGraphicsPipelineParams* pParams)
{
	uint64 hash = LeHash::hash64_bin(pParams, sizeof(*pParams));

	auto Result = m_GraphicsPipelines.find(hash);
	if (Result != m_GraphicsPipelines.end())
		return (*Result).second;

	const LeVkGraphicsPipeline* pGraphicsPipeline = new LeVkGraphicsPipeline(pParams);
	m_GraphicsPipelines[hash] = pGraphicsPipeline;

	return pGraphicsPipeline;
}

void LeVkGraphicsPipelineDictionary::PurgeAllGraphicsPipelines(void)
{
	for (auto pipeline : m_GraphicsPipelines)
		delete pipeline.second;

	m_GraphicsPipelines.clear();
}