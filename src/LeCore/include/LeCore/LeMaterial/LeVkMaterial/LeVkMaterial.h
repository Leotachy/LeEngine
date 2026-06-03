

#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"


class LeVkMaterial : public LeMaterial
{

public:
	LeVkMaterial();
	virtual ~LeVkMaterial();

	void OnDoubleSidedChanged(void) override;

	const LeVkGraphicsPipeline* GetGraphicsPipeline(void) { return m_pGraphicsPipeline; }
	void SetGraphicsPipeline(const LeVkGraphicsPipeline* pPipeline) { m_pGraphicsPipeline = pPipeline; }

	VkDescriptorSet GetDescriptorSet(void) { return m_hDescriptorSet; }
	void SetDescriptorSet(VkDescriptorSet hDescriptorSet) { m_hDescriptorSet = hDescriptorSet; }

	LeBool IsGraphicsPipelineRefreshPending(void) const { return m_bRefreshGraphicsPipeline; }
	void UnsetRefreshGraphicsPipeline(void) { m_bRefreshGraphicsPipeline = false; }

private:
	const LeVkGraphicsPipeline* m_pGraphicsPipeline;
	VkDescriptorSet m_hDescriptorSet;

	LeBool m_bRefreshGraphicsPipeline;

};