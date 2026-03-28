
#pragma once

struct LeVkPhysicalDevice
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

class LeVkPhysicalDevices
{
public:
	LeVkPhysicalDevices() {}
	~LeVkPhysicalDevices() {}

	void Init(const VkInstance& Instance, const VkSurfaceKHR& Surface);
	void DeInit();

	uint32 SelectDevice(VkQueueFlags RequiredQueueType, LeBool SupportsPresent);

	const LeVkPhysicalDevice* Selected() const;

private:
	std::vector<LeVkPhysicalDevice> m_devices;

	int m_devIndex = -1;
};

