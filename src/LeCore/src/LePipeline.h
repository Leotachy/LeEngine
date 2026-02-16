
#pragma once

namespace Le
{
	class LePipeline
	{
	public:

		LePipeline(const char* vertFilePath, const char* fragFilePath);
		~LePipeline();

	private:


		void SetupGraphicsPipeline(const char* vertFilePath, const char* fragFilePath);


		// Caller must free
		const char* ReadShaderBinaryFile(const char* Path, unsigned int& Size);
		

		const char* m_VertShaderBin;
		unsigned int m_VertShaderLen;

		const char* m_FragShaderBin;
		unsigned int m_FragShaderLen;


	};
};