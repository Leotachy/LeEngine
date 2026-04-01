

#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMaterial.h"


class LeMaterialVk : public LeMaterial
{

public:
	LeMaterialVk();
	virtual ~LeMaterialVk();

	const LeVkGraphicsPipeline* GetGraphicsPipeline(void) { return m_graphicsPipeline; }
	void SetGraphicsPipeline(LeVkGraphicsPipeline* pPipeline) { m_graphicsPipeline = pPipeline; }

	LeVkDescriptorSetMaterial* GetLeDescriptorSet(void) { return &m_descriptorSet; }

	void UpdateDescriptorSet(void);

private:
	LeVkGraphicsPipeline* m_graphicsPipeline;

	LeVkDescriptorSetMaterial m_descriptorSet;
};