
#pragma once



/*
*   Wrapper class for VkShaderModule
*/



class LeShader
{
public:

    LeShader();
    ~LeShader();

    void Init(VkDevice Device, const char* pShaderBinaryPath);
    void Destroy();

    VkShaderModule GetShaderModule(void) { return m_shaderModule; }

private:

    LeBool CreateShaderModule(const uint32* pBinaryCode, uint32 codeSize);

    VkShaderModule m_shaderModule;

    VkDevice m_device;
};