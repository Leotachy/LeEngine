
#pragma once

#include "LeCore/LeTexture/LeTexture.h" 

// April 25th, 2026

// To avoid making a sampler for every texture
// store unique samplers within the dictionary
// and get them via a key


class LeVkSamplerDictionary
{

public:
	void Init(void);
	void DeInit(void);

	VkSampler GetOrCreateSampler(const LeTextureAttributes* pAttribs);
	VkSampler GetOrCreateSampler(const LeVkTextureAttributes* pAttribs);

private:

	void PurgeAllSamplers(void);

	std::unordered_map<uint64, VkSampler> m_SamplerSet;
};