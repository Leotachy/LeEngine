

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeShader.h"

#include "LeRender/LeRenderUtils.h"



// temp func until LeStore is done
// TODO
// FIXME
static std::vector<char> readFile(const char* pShaderBinaryPath)
{
	std::ifstream file(pShaderBinaryPath, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		LE_ERROR("Failed to open file %s", pShaderBinaryPath);
	}

	std::vector<char> buffer(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
	file.close();
	return buffer;
}


LeShader::LeShader()
{
	m_device = NULL;
	m_shaderModule = NULL;
}

LeShader::~LeShader()
{
	Destroy();
}

void LeShader::Init(VkDevice* pDevice, const char* pShaderBinaryPath)
{
	m_device = pDevice;
	LE_ASSERT(m_device);

	std::vector<char> code = readFile(pShaderBinaryPath);
	if (code.size())
	{
		LeBool bCreated = CreateShaderModule(reinterpret_cast<const uint32*>(code.data()), uint32(code.size()));
		LE_ASSERT(bCreated);
	}

}

void LeShader::Destroy()
{
	if (!m_device)
		return; // was not initialized

	if (m_shaderModule)
	{
		vkDestroyShaderModule(*m_device, m_shaderModule, NULL);
		m_shaderModule = NULL;
	}
}

LeBool LeShader::CreateShaderModule(const uint32* pBinaryCode, uint32 codeSize)
{
	VkShaderModuleCreateInfo CreateInfo{};
	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.codeSize = codeSize;
	CreateInfo.pCode = pBinaryCode;

	VkResult res = vkCreateShaderModule(*m_device, &CreateInfo, NULL, &m_shaderModule);

	if (res == VK_SUCCESS)
	{
		return true;
	}


	LE_CHECK_VK_RESULT(res, "CreateShaderModule");
	return false;
}