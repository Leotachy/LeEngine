

#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"


class LeVkMaterial : public LeMaterial
{

public:
	LeVkMaterial();
	virtual ~LeVkMaterial();

	const LeVkGraphicsPipeline* GetGraphicsPipeline(void) { return m_pGraphicsPipeline; }
	void SetGraphicsPipeline(LeVkGraphicsPipeline* pPipeline) { m_pGraphicsPipeline = pPipeline; }

	VkDescriptorSet GetDescriptorSet(void) { return m_hDescriptorSet; }
	void SetDescriptorSet(VkDescriptorSet hDescriptorSet) { m_hDescriptorSet = hDescriptorSet; }

private:
	LeVkGraphicsPipeline* m_pGraphicsPipeline;
	VkDescriptorSet m_hDescriptorSet;
};