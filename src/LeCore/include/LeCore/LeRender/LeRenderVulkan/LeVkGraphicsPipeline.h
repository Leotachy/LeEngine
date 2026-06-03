
#pragma once

#include "LeCore/LeShader/LeVkShader/LeVkShader.h"

// an attribute struct that I hash, works as a key for the graphics pipeline dictionary
// must be a trivial struct!
struct LeVkGraphicsPipelineParams
{
	LeVkShader* pVertexShader;
	LeVkShader* pFragmentShader;
	LeBool bDoubleSided;
};

class LeVkGraphicsPipeline
{

public:

	LeVkGraphicsPipeline(const LeVkGraphicsPipelineParams* pParams);
	~LeVkGraphicsPipeline();

	VkPipeline GetPipeline() const { return m_hPipeline; }
	VkPipelineLayout GetPipelineLayout() const { return m_hPipelineLayout; }

private:

	VkPipeline m_hPipeline;
	VkPipelineLayout m_hPipelineLayout;
};

class LeVkGraphicsPipelineDictionary
{
public:

	void Init();
	void DeInit();

	const LeVkGraphicsPipeline* GetOrCreateGraphicsPipeline(const LeVkGraphicsPipelineParams* pParams);


private:

	void PurgeAllGraphicsPipelines(void);

	std::unordered_map<int64, const LeVkGraphicsPipeline*> m_GraphicsPipelines;

};