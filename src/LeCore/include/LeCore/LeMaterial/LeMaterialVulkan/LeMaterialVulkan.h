

#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"

#include "LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"

class LeMaterialVk : public LeMaterial
{

public:
	LeMaterialVk();
	virtual ~LeMaterialVk();

	const LeVkGraphicsPipeline* GetGraphicsPipeline(void) { return m_graphicsPipeline; }
	void SetGraphicsPipeline(LeVkGraphicsPipeline* pPipeline) { m_graphicsPipeline = pPipeline; }

private:
	LeVkGraphicsPipeline* m_graphicsPipeline;



};