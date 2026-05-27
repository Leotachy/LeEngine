
#pragma once



/*
*   Wrapper class for VkShaderModule
*/

#include "LeCore/LeShader/LeShader.h"

class LeVkShader : public LeShader
{
public:

    LeVkShader(const LeChar* name, const LeChar* pShaderFilepath);
    ~LeVkShader(void);

    VkShaderModule CreateShaderModule(const LeByte* ShaderBinary, uint32 ShaderBinarySize);

    VkShaderModule GetShaderModule(void) { return m_hShaderModule; }


private:

   
    VkDevice m_hDevice;
    VkShaderModule m_hShaderModule;

};