
#pragma once

struct LeVkPhysicalDevice
{
	VkPhysicalDevice hPhysDevice;
	VkPhysicalDeviceProperties hDevProps;
	std::vector<VkQueueFamilyProperties> QueueFamilyProps;
	std::vector<VkBool32> QueueSupportsPresent;
	std::vector<VkSurfaceFormatKHR> SurfaceFormats;
	VkSurfaceCapabilitiesKHR SurfaceCaps;
	VkPhysicalDeviceMemoryProperties MemProps;
	std::vector<VkPresentModeKHR> PresentModes;
	VkPhysicalDeviceFeatures Features;
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
	std::vector<LeVkPhysicalDevice> m_Devices;

	int m_DeviceIndex = -1;
};

