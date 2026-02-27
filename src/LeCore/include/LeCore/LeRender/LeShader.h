
#pragma once

/*

    Created by Lukas Andrijauskas
    27th February, 2026

    LeShader - a wrapper class for VkShaderModule

*/

class LeShader
{
public:

    LeShader();
    ~LeShader();

    void Init(VkDevice* pDevice, const char* pShaderBinaryPath);
    void Destroy();

private:

    LeBool CreateShaderModule(const uint32* pBinaryCode, uint32 codeSize);

    VkDevice* m_device;
    VkShaderModule m_shaderModule;

};