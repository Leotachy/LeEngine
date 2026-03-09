
#include "LeCore/LeMesh/LeSimpleMesh.h"

class LeGraphicsPipeline
{

public:

	LeGraphicsPipeline();
	~LeGraphicsPipeline();

	void Init(VkDevice Device, const LeSwapchain* pSwapchain, LeShader* pShader, const LeSimpleMesh* pSimpleMesh,
		int32 NumImages);
	void Destroy(void);

	void Bind(VkCommandBuffer CmdBuf, int32 ImageIndex);

private:

	void CreateDescriptorPool(int32 NumImages);
	void CreateDescriptorSets(const LeSimpleMesh* pMesh, int32 NumImages);
	void CreateDescriptorSetLayout(void);
	void AllocateDescriptorSets(int32 NumImages);
	void UpdateDescriptorSets(const LeSimpleMesh* pMesh, int32 NumImages);

	VkPipeline m_pipeline;
	VkPipelineLayout m_pipelineLayout;

	VkDescriptorPool m_descriptorPool;
	VkDescriptorSetLayout m_descriptorSetLayout;
	std::vector<VkDescriptorSet> m_descriptorSets;

	VkDevice m_device;
};

