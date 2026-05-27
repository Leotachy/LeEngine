
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeShader/LeVkShader/LeVkShader.h"
#include "LeCore/LeStream/LeStreamFile.h"


LeVkShader::LeVkShader(const LeChar* name, const LeChar* pShaderFilepath) : LeShader(name, pShaderFilepath)
{
	m_hDevice = LeVkRenderer::Get()->GetDevice();
	LE_DEBUG_ASSERT(m_hDevice);

	LeByte ShaderBinary[128000];

	LeStreamFile ShaderFileStream;
	ShaderFileStream.Open(LeStreamAccessType::READ, pShaderFilepath);
	uint32 ShaderBinarySize = ShaderFileStream.Read(ShaderBinary, sizeof(ShaderBinary));
	if (ShaderBinarySize == sizeof(ShaderBinary))
	{
		LE_WARN("Shader %s at path %s is too big for buffer", name, pShaderFilepath);
	}


	CreateShaderModule(ShaderBinary, ShaderBinarySize);
}

LeVkShader::~LeVkShader(void)
{
	if (m_hDevice)
	{
		if (m_hShaderModule)
		{
			vkDestroyShaderModule(m_hDevice, m_hShaderModule, NULL);
		}
	}
}

VkShaderModule LeVkShader::CreateShaderModule(const LeByte* ShaderBinary, uint32 ShaderBinarySize)
{

	if (ShaderBinarySize == 0)
	{
		LE_ERROR("No binary to create a shader module from!");
		return VK_NULL_HANDLE;
	}

	VkShaderModuleCreateInfo CreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = ShaderBinarySize,
		.pCode = reinterpret_cast<const uint32*>(ShaderBinary)
	};


	VkResult res = vkCreateShaderModule(m_hDevice, &CreateInfo, NULL, &m_hShaderModule);
	LE_CHECK_VK_RESULT(res);

	return m_hShaderModule;
}