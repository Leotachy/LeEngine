

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
	m_shaderModule = NULL;
	m_device = NULL;
}

LeShader::~LeShader()
{
}

void LeShader::Init(VkDevice Device, const char* pShaderBinaryPath)
{
	m_device = Device;
	LE_DEBUG_ASSERT(m_device);

	std::vector<char> code = readFile(pShaderBinaryPath);
	if (code.size())
	{
		LeBool bCreated = CreateShaderModule(reinterpret_cast<const uint32*>(code.data()), uint32(code.size()));
		LE_ASSERT(bCreated);

		LE_DEBUG_MESSAGE("Shader created");
	}

}

void LeShader::Destroy()
{

	vkDestroyShaderModule(m_device, m_shaderModule, NULL);
	m_shaderModule = NULL;

	LE_DEBUG_MESSAGE("Shader destroyed");

	m_device = NULL;
}

LeBool LeShader::CreateShaderModule(const uint32* pBinaryCode, uint32 codeSize)
{
	VkShaderModuleCreateInfo CreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = codeSize,
		.pCode = pBinaryCode
	};


	VkResult res = vkCreateShaderModule(m_device, &CreateInfo, NULL, &m_shaderModule);

	if (res == VK_SUCCESS)
	{
		return true;
	}


	LE_CHECK_VK_RESULT(res, "CreateShaderModule");
	return false;
}