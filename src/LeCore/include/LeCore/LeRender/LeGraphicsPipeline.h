


class LeGraphicsPipeline
{

public:

	LeGraphicsPipeline();
	~LeGraphicsPipeline();

	void Init(VkDevice Device, const LeSwapchain* pSwapchain, LeShader* pShader);
	void Destroy(void);

	void Bind(VkCommandBuffer CmdBuf);

private:

	VkPipeline m_pipeline;
	VkPipelineLayout m_pipelineLayout;

	VkDevice m_device;
};

