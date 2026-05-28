
#pragma once

enum class LeShaderType
{
	Vertex,
	Fragment,

	NumShaderTypes
};

class LeShader
{
protected:
	LeShader(const char* pName, const char* pShaderBinaryPath, LeShaderType type);
public:
	virtual ~LeShader();
	static LeShader* Create(const char* pName, const char* pShaderBinaryPath, LeShaderType type);

	uint64 GetNameHash(void) { return m_NameHash; }
	LeShaderType GetType(void) { return m_ShaderType; }


	static LeShaderType GetShaderTypeByFileExtension(const LeChar* pExtension);

protected:

	uint64 m_NameHash;
	LeShaderType m_ShaderType;
};