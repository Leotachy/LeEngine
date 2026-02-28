


class LeGraphicsPipeline
{

public:

	LeGraphicsPipeline(VkDevice Device, const LeSwapchain* pSwapchain, LeShader* pShader);
	~LeGraphicsPipeline();

	void Bind(VkCommandBuffer CmdBuf);

private:

	VkPipeline m_pipeline = NULL;
	VkPipelineLayout m_pipelineLayout = NULL;

	VkDevice m_device = NULL;
};

