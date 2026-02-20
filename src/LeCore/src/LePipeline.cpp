
#include "lecore/lecore.h"
#include "LePipeline.h"



LePipelineConfigInfo LePipeline::DefaultPipelineConfigInfo(uint32 Width, uint32 Height)
{
	LePipelineConfigInfo ConfigInfo{};
	return ConfigInfo;
}


LePipeline::LePipeline(const LePipelineConfigInfo& ConfigInfo, LeDevice& Device, const LeChar* vertFilePath, const LeChar* fragFilePath)
	: m_LeDevice{ Device }
{
	m_VertShaderBin = nullptr;
	m_VertShaderLen = 0;

	m_FragShaderBin = nullptr;
	m_FragShaderLen = 0;

	CreateGraphicsPipeline(ConfigInfo, vertFilePath, fragFilePath);
}

LePipeline::~LePipeline()
{
	delete m_VertShaderBin;
	delete m_FragShaderBin;
}

void LePipeline::CreateGraphicsPipeline(const LePipelineConfigInfo& ConfigInfo, const LeChar* vertFilePath, const LeChar* fragFilePath)
{
	m_VertShaderBin = ReadShaderBinaryFile(vertFilePath, m_VertShaderLen);
	LE_DEBUG_MESSAGE("Vertex shader %s binary length %u", vertFilePath, m_VertShaderLen);
	LE_DEBUG_ASSERT(m_VertShaderLen % sizeof(uint32) == 0);

	m_FragShaderBin = ReadShaderBinaryFile(fragFilePath, m_FragShaderLen);
	LE_DEBUG_MESSAGE("Fragment shader %s binary length %u", fragFilePath, m_FragShaderLen);
	LE_DEBUG_ASSERT(m_FragShaderLen % sizeof(uint32) == 0);
}

void LePipeline::CreateShaderModule(const uint32* Bin, const uint32 Size, VkShaderModule* ShaderModule)
{
	VkShaderModuleCreateInfo CreateInfo{};
	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.codeSize = Size;
	CreateInfo.pCode = Bin;

	if (vkCreateShaderModule(m_LeDevice.device(), &CreateInfo, nullptr, ShaderModule) != VK_SUCCESS)
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

