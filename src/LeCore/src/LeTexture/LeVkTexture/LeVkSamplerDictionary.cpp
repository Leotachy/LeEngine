
#include "LeCore/LeCore.h"

#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkSamplerDictionary.h"


void LeVkSamplerDictionary::Init(void)
{
}

void LeVkSamplerDictionary::DeInit(void)
{
	PurgeAllSamplers();
}

VkSampler LeVkSamplerDictionary::GetOrCreateSampler(const LeTextureAttributes* pAttribs)
{
	LE_DEBUG_ASSERT(pAttribs);

	LeVkTextureAttributes VkAttribs(pAttribs);

	return GetOrCreateSampler(&VkAttribs);
}

VkSampler LeVkSamplerDictionary::GetOrCreateSampler(const LeVkTextureAttributes* pAttribs)
{
	LE_DEBUG_ASSERT(pAttribs);

	uint64 hash = lehash64_bin(pAttribs, sizeof(*pAttribs));

	auto Result = m_SamplerSet.find(hash);
	if (Result != m_SamplerSet.end())
		return (*Result).second;

	const LeVkPhysicalDevice* pPhysicalDevice = LeVkCore::Singleton().GetLePhysicalDevice();
	LE_DEBUG_ASSERT(pPhysicalDevice);
	VkSampler NewSampler = LeVkUtils::CreateSampler(pAttribs, true, pPhysicalDevice->m_devProps.limits.maxSamplerAnisotropy);

	m_SamplerSet[hash] = NewSampler;

	return NewSampler;
}

void LeVkSamplerDictionary::PurgeAllSamplers(void)
{
	for (auto pair : m_SamplerSet)
	{
		VkSampler Sampler = pair.second;
		vkDestroySampler(LeVkCore::Singleton().GetDevice(), Sampler, NULL);
	}

	m_SamplerSet.clear();
}