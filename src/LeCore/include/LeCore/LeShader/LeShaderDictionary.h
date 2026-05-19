
#pragma once

#include "LeCore/LeShader/LeShader.h"

class LeShaderDictionary
{
public:

	LeShaderDictionary(void);
	~LeShaderDictionary(void);

	LeShader* FindShader(uint64 uNameHash) const;
	LeBool ShaderExists(uint64 uNameHash) const;
	
	void LoadAllShaders(const LeChar* pDirectoryPath);
	void PurgeAllShaders(void);

private:

	LeBool AddShader(LeShader* pShader);

	std::unordered_map<uint64, LeShader*> m_ShaderSet;

};