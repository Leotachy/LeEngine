

#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMaterial.h"


class LeVkMaterial : public LeMaterial
{

public:
	LeVkMaterial();
	virtual ~LeVkMaterial();

	const LeVkGraphicsPipeline* GetGraphicsPipeline(void) { return m_pGraphicsPipeline; }
	void SetGraphicsPipeline(LeVkGraphicsPipeline* pPipeline) { m_pGraphicsPipeline = pPipeline; }

	LeVkDescriptorSetMaterial* GetLeDescriptorSet(void) { return &m_DescriptorSet; }

	void UpdateDescriptorSet(void);

private:
	LeVkGraphicsPipeline* m_pGraphicsPipeline;

	LeVkDescriptorSetMaterial m_DescriptorSet;
};