
#include "LeCore/LeCore.h"

#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkSamplerDictionary.h"


void LeVkSamplerDictionary::Init(VkDevice device, const LeVkPhysicalDevice* pPhysicalDevice)
{
	LE_DEBUG_ASSERT(device != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(pPhysicalDevice);

	m_device = device;
	m_physicalDevice = pPhysicalDevice;
}

void LeVkSamplerDictionary::DeInit(void)
{
	PurgeAllSamplers();

	m_device = VK_NULL_HANDLE;
	m_physicalDevice = NULL;
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


	VkSampler NewSampler = LeVkUtils::CreateSampler(LeVkCore::Singleton().GetDevice(),
		pAttribs, true, m_physicalDevice->m_devProps.limits.maxSamplerAnisotropy);
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