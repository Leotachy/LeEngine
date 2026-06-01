

#pragma once

class LeVkDescriptor
{
public:
	void Init(const VkDescriptorType* pDescriptorTypes, uint32 NumTypes, VkShaderStageFlags ShaderStageFlags, uint32 MaxSets);
	void DeInit(void);

	void Reset(uint32 FrameIndex);

	VkDescriptorSetLayout GetDescriptorSetLayout(void);

	virtual VkDescriptorSet CreateDescriptorSet(uint32 FrameIndex, void* pData);



private:

	struct DescriptorPool
	{
		VkDescriptorPool hPool;
		uint32 NumSetsActive;
	};

	DescriptorPool m_Pools[LE_MAX_FRAMES_IN_FLIGHT];
	VkDescriptorSetLayout m_hLayout;
	uint32 m_MaxSetsPerPool;
};

class LeVkFrameDescriptor : public LeVkDescriptor
{
public:
	VkDescriptorSet CreateDescriptorSet(uint32 FrameIndex, void* pData) override;
};

class LeVkMeshDescriptor : public LeVkDescriptor
{
public:
	VkDescriptorSet CreateDescriptorSet(uint32 FrameIndex, void* pData) override;
};

struct MaterialDescriptorSetParams
{
	struct TextureInfo
	{
		VkSampler hSampler;
		VkImageView hImageView;
	};

	TextureInfo BaseTextureInfo;

};
class LeVkMaterialDescriptor : public LeVkDescriptor
{
public:
	VkDescriptorSet CreateDescriptorSet(uint32 FrameIndex, void* pData) override;
};

enum class LeVkDescriptorType
{
	Frame,
	Mesh,
	Material,

	NumDescriptorTypes
};

class LeVkDescriptorManager
{
public:
	void Init(void);
	void DeInit(void);

	void ResetDescriptors(uint32 FrameIndex);

	LeVkDescriptor* GetDescriptor(LeVkDescriptorType type);

private:

	LeVkFrameDescriptor m_FrameData;
	LeVkMeshDescriptor m_MeshData;
	LeVkMaterialDescriptor m_MaterialData;

};