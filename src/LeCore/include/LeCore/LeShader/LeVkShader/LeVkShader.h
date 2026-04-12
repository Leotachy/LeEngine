
#pragma once



/*
*   Wrapper class for VkShaderModule
*/

#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"
#include "LeCore/LeShader/LeShader.h"

class LeVkShader : public LeShader
{
public:

    LeVkShader(const LeChar* name, const LeChar* pShaderFilepath);
    ~LeVkShader(void);

    VkShaderModule CreateShaderModule(const LeByte* ShaderBinary, uint32 ShaderBinarySize);

    VkShaderModule GetShaderModule(void) { return m_ShaderModule; }


private:

   
    VkDevice m_Device;
    VkShaderModule m_ShaderModule;

};