
#include "LePipeline.h"
#include <fstream>

namespace Le
{

	LePipeline::LePipeline(const char* vertFilePath, const char* fragFilePath)
	{
		m_VertShaderBin = nullptr;
		m_VertShaderLen = 0;
		
		m_FragShaderBin = nullptr;
		m_FragShaderLen = 0;

		SetupGraphicsPipeline(vertFilePath, fragFilePath);
	}

	LePipeline::~LePipeline()
	{
		delete m_VertShaderBin;
		delete m_FragShaderBin;
	}

	void LePipeline::SetupGraphicsPipeline(const char* vertFilePath, const char* fragFilePath)
	{
		m_VertShaderBin = ReadShaderBinaryFile(vertFilePath, m_VertShaderLen);
		printf_s("Vertex shader bin len: %u\n", m_VertShaderLen);

		m_FragShaderBin = ReadShaderBinaryFile(fragFilePath, m_FragShaderLen);
		printf_s("Fragment shader bin len: %u\n", m_FragShaderLen);
	}

	const char* LePipeline::ReadShaderBinaryFile(const char* Path, unsigned int& Size)
	{

		std::ifstream file{ Path, std::ios::ate | std::ios::binary };

		if (!file.is_open())
		{
			char buf[256];
			sprintf_s(buf, sizeof(buf), "failed to open file: %s\n", Path);
			// TODO: fire assert here
			Size = 0;
			return nullptr;
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		Size = int(fileSize);
		char* DataBuffer = (char*)malloc(fileSize * sizeof(char));

		file.seekg(0);
		file.read(DataBuffer, fileSize);

		file.close();
		return DataBuffer;

	}

};