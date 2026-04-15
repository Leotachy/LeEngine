
#pragma once

class LeShader
{
protected:
	LeShader(const char* pName, const char* pShaderBinaryPath);
public:
	virtual ~LeShader();
	static LeShader* Create(const char* pName, const char* pShaderBinaryPath);


	uint64 GetNameHash(void) { return m_NameHash; }



protected:

	uint64 m_NameHash;
};