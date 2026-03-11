
#pragma once



/*
*   Wrapper class for VkShaderModule
*/

#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeShader/LeShader.h"

class LeShaderVk : public LeShader
{
public:

    LeShaderVk(const LeChar* name, const LeChar* pShaderFilepath);
    ~LeShaderVk(void);

    VkShaderModule CreateShaderModule(std::vector<LeByte>& ShaderBinary);

    VkShaderModule GetShaderModule(void) { return m_ShaderModule; }


private:

   
    VkDevice m_Device;
    VkShaderModule m_ShaderModule;

};