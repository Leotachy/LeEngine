
#pragma once

#include "LeCore/LeShader/LeVkShader/LeVkShader.h"



class LeVkGraphicsPipeline
{

public:

	LeVkGraphicsPipeline(uint64 key, int32 NumImages, LeVkShader* pShader, VkFormat RenderColFormat);
	~LeVkGraphicsPipeline();

	uint64 GetKey() const { return m_key; }

	VkPipeline GetPipeline() const { return m_pipeline; }
	VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }

private:

	uint64 m_key; // Key in dictionary

	VkDevice m_device;

	VkPipeline m_pipeline;
	VkPipelineLayout m_pipelineLayout;




};

// an attribute struct that I hash, works as a key for the graphics pipeline dictionary
struct LeGraphicsPipelineVkKey
{
	// swapchain
	int32 NumImages;
	VkFormat RenderColFormat;

	// material
	uint64 ShaderHash;
};

class LeVkGraphicsPipelineDictionary
{

	LeVkGraphicsPipelineDictionary() {}
	~LeVkGraphicsPipelineDictionary() {}

public:

	static LeVkGraphicsPipelineDictionary* Singleton();

	void Add(LeVkGraphicsPipeline* pPipeline);
	LeVkGraphicsPipeline* Find(int64 key);
	void Purge(void);



private:



	std::unordered_map<int64, LeVkGraphicsPipeline*> m_GraphicsPipelines;

};