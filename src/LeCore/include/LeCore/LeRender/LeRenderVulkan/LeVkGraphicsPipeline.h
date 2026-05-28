
#pragma once

#include "LeCore/LeShader/LeVkShader/LeVkShader.h"



class LeVkGraphicsPipeline
{

public:

	LeVkGraphicsPipeline(uint64 key, LeVkShader* pVertexShader, LeVkShader* pFragmentShader);
	~LeVkGraphicsPipeline();

	uint64 GetKey() const { return m_Key; }

	VkPipeline GetPipeline() const { return m_hPipeline; }
	VkPipelineLayout GetPipelineLayout() const { return m_hPipelineLayout; }

private:

	uint64 m_Key; // Key in dictionary

	VkPipeline m_hPipeline;
	VkPipelineLayout m_hPipelineLayout;
};

// an attribute struct that I hash, works as a key for the graphics pipeline dictionary
struct LeGraphicsPipelineVkKey
{
	// material
	uint64 VertexShaderHash;
	uint64 FragmentShaderHash;
};

class LeVkGraphicsPipelineDictionary
{

	LeVkGraphicsPipelineDictionary() {}
	~LeVkGraphicsPipelineDictionary() {}

public:

	static LeVkGraphicsPipelineDictionary* Get();

	void Add(LeVkGraphicsPipeline* pPipeline);
	LeVkGraphicsPipeline* Find(int64 key);
	void Purge(void);



private:



	std::unordered_map<int64, LeVkGraphicsPipeline*> m_GraphicsPipelines;

};