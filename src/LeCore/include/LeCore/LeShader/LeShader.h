
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

class LeShaderDictionary
{
public:

	void AddShader(LeShader* pShader);
	LeShader* GetShader(uint64 uNameHash);
	LeBool ShaderExists(uint64 uNameHash);



	void PurgeAllShaders(void);


private:


	std::unordered_map<uint64, LeShader*> m_ShaderSet;

};

LeShaderDictionary* LeGetCurrentShaderDictionary(void);