
#pragma once
#include "lecore/lecore.h"
#include "LeDevice.h"



struct LePipelineConfigInfo
{

};

class LePipeline
{
public:

	static LePipelineConfigInfo DefaultPipelineConfigInfo(uint32 Width, uint32 Height);

	LePipeline(const LePipelineConfigInfo& ConfigInfo, LeDevice& Device, const LeChar* vertFilePath, const LeChar* fragFilePath);
	~LePipeline();

	LePipeline(const LePipeline&) = delete;
	void operator=(const LePipeline&) = delete;

private:


	void CreateGraphicsPipeline(const LePipelineConfigInfo& ConfigInfo, const LeChar* vertFilePath, const LeChar* fragFilePath);
	void CreateShaderModule(const uint32* Bin, const uint32 Size, VkShaderModule* ShaderModule);

	// Caller must free
	const uint32* ReadShaderBinaryFile(const LeChar* Path, uint32& Size /* Returns size in bytes */);
		

	const uint32* m_VertShaderBin;
	uint32 m_VertShaderLen;

	const uint32* m_FragShaderBin;
	uint32 m_FragShaderLen;

	LeDevice& m_LeDevice;
	VkPipeline m_GraphicsPipeline;
	VkShaderModule m_VertShaderModule;
	VkShaderModule m_FragShaderModule;


};
