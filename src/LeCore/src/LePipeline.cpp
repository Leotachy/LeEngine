
#include "lecore/lecore.h"
#include "LePipeline.h"









LePipelineConfigInfo LePipeline::DefaultPipelineConfigInfo(uint32 Width, uint32 Height)
{
	LePipelineConfigInfo ConfigInfo{};

    ConfigInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ConfigInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ConfigInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    ConfigInfo.viewport.x = 0.0f;
    ConfigInfo.viewport.y = 0.0f;
    ConfigInfo.viewport.width = static_cast<float32>(Width);
    ConfigInfo.viewport.height = static_cast<float32>(Height);
    ConfigInfo.viewport.minDepth = 0.0f;
    ConfigInfo.viewport.maxDepth = 1.0f;

    ConfigInfo.scissor.offset = { 0, 0 };
    ConfigInfo.scissor.extent = { Width, Height };

    ConfigInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    ConfigInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    ConfigInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    ConfigInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    ConfigInfo.rasterizationInfo.lineWidth = 1.0f;
    ConfigInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    ConfigInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    ConfigInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    ConfigInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
    ConfigInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
    ConfigInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

    ConfigInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ConfigInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    ConfigInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    ConfigInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
    ConfigInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
    ConfigInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
    ConfigInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

    ConfigInfo.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    ConfigInfo.colorBlendAttachment.blendEnable = VK_FALSE;
    ConfigInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    ConfigInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    ConfigInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    ConfigInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    ConfigInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    ConfigInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    ConfigInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ConfigInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    ConfigInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    ConfigInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    ConfigInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    ConfigInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
    ConfigInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
    ConfigInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    ConfigInfo.depthStencilInfo.front = {};  // Optional
    ConfigInfo.depthStencilInfo.back = {};   // Optional

    return ConfigInfo;
}


LePipeline::LePipeline()
	: m_Inited{false}
{
	m_VertShaderBin = nullptr;
	m_VertShaderLen = 0;

	m_FragShaderBin = nullptr;
	m_FragShaderLen = 0;

    m_LeDevice = nullptr;

    m_GraphicsPipeline = VK_NULL_HANDLE;

    m_VertShaderModule = VK_NULL_HANDLE;
    m_FragShaderModule = VK_NULL_HANDLE;
}

LePipeline::~LePipeline()
{
    DeInit();
}

void LePipeline::Init(const LePipelineConfigInfo& ConfigInfo, LeDevice& Device, const LeChar* vertFilePath, const LeChar* fragFilePath)
{
    m_Inited = true;

    m_LeDevice = &Device;

    CreateGraphicsPipeline(ConfigInfo, vertFilePath, fragFilePath);

}

void LePipeline::DeInit()
{
    if (m_Inited)
    {
        delete m_VertShaderBin;
        delete m_FragShaderBin;

        vkDestroyShaderModule(m_LeDevice->device(), m_VertShaderModule, nullptr);
        vkDestroyShaderModule(m_LeDevice->device(), m_FragShaderModule, nullptr);
        vkDestroyPipeline(m_LeDevice->device(), m_GraphicsPipeline, nullptr);

        m_Inited = false;
    }
}

void LePipeline::Bind(VkCommandBuffer CommandBuffer)
{
    LE_DEBUG_ASSERT(m_Inited);

    vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
}

void LePipeline::CreateGraphicsPipeline(const LePipelineConfigInfo& ConfigInfo, const LeChar* vertFilePath, const LeChar* fragFilePath)
{

    LE_DEBUG_ASSERT(ConfigInfo.pipelineLayout != VK_NULL_HANDLE &&
        "Cannot create graphics pipeline: no pipeline layout provided in config info");

    LE_DEBUG_ASSERT(ConfigInfo.renderPass != VK_NULL_HANDLE &&
        "Cannot create graphics pipeline: no render pass provided in config info");
	
    m_VertShaderBin = ReadShaderBinaryFile(vertFilePath, m_VertShaderLen);
	LE_DEBUG_MESSAGE("Vertex shader %s binary length %u", vertFilePath, m_VertShaderLen);
	LE_DEBUG_ASSERT(m_VertShaderLen % sizeof(uint32) == 0);
    CreateShaderModule(m_VertShaderBin, m_VertShaderLen, &m_VertShaderModule);

	m_FragShaderBin = ReadShaderBinaryFile(fragFilePath, m_FragShaderLen);
	LE_DEBUG_MESSAGE("Fragment shader %s binary length %u", fragFilePath, m_FragShaderLen);
	LE_DEBUG_ASSERT(m_FragShaderLen % sizeof(uint32) == 0);
    CreateShaderModule(m_FragShaderBin, m_FragShaderLen, &m_FragShaderModule);

    VkPipelineShaderStageCreateInfo ShaderStages[2];
    ShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    ShaderStages[0].module = m_VertShaderModule;
    ShaderStages[0].pName = "main";
    ShaderStages[0].flags = 0;
    ShaderStages[0].pNext = nullptr;
    ShaderStages[0].pSpecializationInfo = nullptr;

    ShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    ShaderStages[1].module = m_FragShaderModule;
    ShaderStages[1].pName = "main";
    ShaderStages[1].flags = 0;
    ShaderStages[1].pNext = nullptr;
    ShaderStages[1].pSpecializationInfo = nullptr;

    VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputInfo.vertexAttributeDescriptionCount = 0;
    VertexInputInfo.pVertexAttributeDescriptions = nullptr;
    VertexInputInfo.vertexBindingDescriptionCount = 0;
    VertexInputInfo.pVertexBindingDescriptions = nullptr;

    VkPipelineViewportStateCreateInfo ViewportInfo{};
    ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportInfo.viewportCount = 1;
    ViewportInfo.pViewports = &ConfigInfo.viewport;
    ViewportInfo.scissorCount = 1;
    ViewportInfo.pScissors = &ConfigInfo.scissor;

    VkPipelineColorBlendStateCreateInfo ColorBlendInfo{};
    ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlendInfo.logicOpEnable = VK_FALSE;
    ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
    ColorBlendInfo.attachmentCount = 1;
    ColorBlendInfo.pAttachments = &ConfigInfo.colorBlendAttachment;
    ColorBlendInfo.blendConstants[0] = 0.0f;  // Optional
    ColorBlendInfo.blendConstants[1] = 0.0f;  // Optional
    ColorBlendInfo.blendConstants[2] = 0.0f;  // Optional
    ColorBlendInfo.blendConstants[3] = 0.0f;  // Optional

    VkGraphicsPipelineCreateInfo PipelineCreateInfo{};
    PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineCreateInfo.stageCount = 2;
    PipelineCreateInfo.pStages = ShaderStages;
    PipelineCreateInfo.pVertexInputState = &VertexInputInfo;
    PipelineCreateInfo.pInputAssemblyState = &ConfigInfo.inputAssemblyInfo;
    PipelineCreateInfo.pViewportState = &ViewportInfo;
    PipelineCreateInfo.pRasterizationState = &ConfigInfo.rasterizationInfo;
    PipelineCreateInfo.pMultisampleState = &ConfigInfo.multisampleInfo;
    PipelineCreateInfo.pDepthStencilState = &ConfigInfo.depthStencilInfo;
    PipelineCreateInfo.pColorBlendState = &ColorBlendInfo;
    PipelineCreateInfo.pDynamicState = nullptr;

    PipelineCreateInfo.layout = ConfigInfo.pipelineLayout;
    PipelineCreateInfo.renderPass = ConfigInfo.renderPass;
    PipelineCreateInfo.subpass = ConfigInfo.subpass;

    PipelineCreateInfo.basePipelineIndex = -1;
    PipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(m_LeDevice->device(), VK_NULL_HANDLE, 1, &PipelineCreateInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
    {
        LE_ERROR("Failed to create vulkan graphics pipeline");
    }

}

void LePipeline::CreateShaderModule(const uint32* Bin, const uint32 Size, VkShaderModule* ShaderModule)
{
	VkShaderModuleCreateInfo CreateInfo{};
	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.codeSize = Size;
	CreateInfo.pCode = Bin;

	if (vkCreateShaderModule(m_LeDevice->device(), &CreateInfo, nullptr, ShaderModule) != VK_SUCCESS)
		LE_ERROR("Failed to create vulkan shader module");


}

const uint32* LePipeline::ReadShaderBinaryFile(const LeChar* Path, uint32& Size)
{

	std::ifstream file{ Path, std::ios::ate | std::ios::binary };

	if (!file.is_open())
	{
		LE_ERROR("Failed to open file: %s", Path);

		Size = 0;
		return nullptr;
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	Size = int(fileSize);
	uint32* DataBuffer = (uint32*)LeMalloc(fileSize * sizeof(LeByte));

	file.seekg(0);
	file.read((LeChar*)DataBuffer, fileSize);

	file.close();
	return DataBuffer;

}

