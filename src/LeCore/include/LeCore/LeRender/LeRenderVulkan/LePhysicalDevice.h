
#pragma once

struct LePhysicalDevice
{
	VkPhysicalDevice m_physDevice;
	VkPhysicalDeviceProperties m_devProps;
	std::vector<VkQueueFamilyProperties> m_qFamilyProps;
	std::vector<VkBool32> m_qSupportsPresent;
	std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
	VkSurfaceCapabilitiesKHR m_surfaceCaps;
	VkPhysicalDeviceMemoryProperties m_memProps;
	std::vector<VkPresentModeKHR> m_presentModes;
	VkPhysicalDeviceFeatures m_features;
};

class LeVulkanPhysicalDevices
{
public:
	LeVulkanPhysicalDevices() {}
	~LeVulkanPhysicalDevices() {}

	void Init(const VkInstance& Instance, const VkSurfaceKHR& Surface);
	void DeInit();

	uint32 SelectDevice(VkQueueFlags RequiredQueueType, LeBool SupportsPresent);

	const LePhysicalDevice& Selected() const;

private:
	std::vector<LePhysicalDevice> m_devices;

	int m_devIndex = -1;
};

