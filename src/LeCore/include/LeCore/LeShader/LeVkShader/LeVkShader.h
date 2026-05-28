
#pragma once



/*
*   Wrapper class for VkShaderModule
*/

#include "LeCore/LeShader/LeShader.h"

class LeVkShader : public LeShader
{
public:

    LeVkShader(const LeChar* name, const LeChar* pShaderFilepath, LeShaderType type);
    ~LeVkShader(void);

    VkShaderModule CreateShaderModule(const LeByte* ShaderBinary, uint32 ShaderBinarySize);

    VkShaderModule GetShaderModule(void) { return m_hShaderModule; }


private:

  
    VkShaderModule m_hShaderModule;

};