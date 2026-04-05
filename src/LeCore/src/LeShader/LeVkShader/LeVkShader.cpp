
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeShader/LeVkShader/LeVkShader.h"

// temp func until LeStore is done
// TODO
// FIXME
static std::vector<LeByte> readShaderBinaryFile(const char* pShaderBinaryPath)
{
	std::ifstream file(pShaderBinaryPath, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		LE_ERROR("Failed to open file %s", pShaderBinaryPath);
	}

	std::vector<LeByte> buffer(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read((LeChar*)buffer.data(), static_cast<std::streamsize>(buffer.size()));
	file.close();
	return buffer;
}

LeVkShader::LeVkShader(const LeChar* name, const LeChar* pShaderFilepath) : LeShader(name, pShaderFilepath)
{
	m_Device = LeVkCore::Singleton().LeVkGetLogicalDevice();
	LE_DEBUG_ASSERT(m_Device);

	std::vector<LeByte> bin = readShaderBinaryFile(pShaderFilepath);
	CreateShaderModule(bin);
}

LeVkShader::~LeVkShader(void)
{
	if (m_Device)
	{
		if (m_ShaderModule)
		{
			vkDestroyShaderModule(m_Device, m_ShaderModule, NULL);
		}
	}
}

VkShaderModule LeVkShader::CreateShaderModule(std::vector<LeByte>& ShaderBinary)
{

	if (ShaderBinary.size() == 0)
	{
		LE_ERROR("No binary to create a shader module from!");
		return VK_NULL_HANDLE;
	}

	VkShaderModuleCreateInfo CreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = ShaderBinary.size(),
		.pCode = reinterpret_cast<const uint32*>(ShaderBinary.data())
	};


	VkResult res = vkCreateShaderModule(m_Device, &CreateInfo, NULL, &m_ShaderModule);
	LE_CHECK_VK_RESULT(res);

	return m_ShaderModule;
}