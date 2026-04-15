
#pragma once

#include "LeCore/LeShader/LeShader.h"

class LeShaderDictionary
{
public:

	LeShaderDictionary(void);
	~LeShaderDictionary(void);

	LeBool AddShader(LeShader* pShader);
	LeShader* FindShader(uint64 uNameHash);
	LeBool ShaderExists(uint64 uNameHash);



	void PurgeAllShaders(void);


private:


	std::unordered_map<uint64, LeShader*> m_ShaderSet;

};