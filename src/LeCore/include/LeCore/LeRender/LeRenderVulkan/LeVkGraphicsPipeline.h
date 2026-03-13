
#pragma once

#include "LeCore/LeShader/LeShaderVulkan/LeShaderVulkan.h"



class LeVkGraphicsPipeline
{

public:

	LeVkGraphicsPipeline(uint64 key, int32 NumImages, LeShaderVk* pShader, VkFormat RenderColFormat);
	~LeVkGraphicsPipeline();

	void Bind(VkCommandBuffer CmdBuf, int32 ImageIndex);

	uint64 GetKey() { return m_key; }

private:

	void CreateDescriptorPool(int32 NumImages);
	void CreateDescriptorSets(const LeSimpleMesh* pMesh, int32 NumImages, std::vector<LeMemoryBuffer>& UniformBuffers,
		int32 UniformDataSize);
	void CreateDescriptorSetLayout(std::vector<LeMemoryBuffer>& UniformBuffers, int32 UniformDataSize);
	void AllocateDescriptorSets(int32 NumImages);
	void UpdateDescriptorSets(const LeSimpleMesh* pMesh, int32 NumImages, std::vector<LeMemoryBuffer>& UniformBuffers,
		int32 UniformDataSize);

	uint64 m_key; // Key in dictionary

	VkDevice m_device;

	VkDescriptorPool m_descriptorPool;
	VkDescriptorSetLayout m_descriptorSetLayout;
	std::vector<VkDescriptorSet> m_descriptorSets;

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
	int64 ShaderHash;
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