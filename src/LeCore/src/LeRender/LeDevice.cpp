
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeDevice.h"
#include "LeRenderUtils.h"



static void PrintImageUsageFlags(const VkImageUsageFlags& flags)
{
    if (flags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        LE_DEBUG_MESSAGE("Image usage transfer src is supported");
    }

    if (flags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        LE_DEBUG_MESSAGE("Image usage transfer dest is supported");
    }

    if (flags & VK_IMAGE_USAGE_SAMPLED_BIT) {
        LE_DEBUG_MESSAGE("Image usage sampled is supported");
    }

    if (flags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        LE_DEBUG_MESSAGE("Image usage color attachment is supported");
    }

    if (flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        LE_DEBUG_MESSAGE("Image usage depth stencil attachment is supported");
    }

    if (flags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
        LE_DEBUG_MESSAGE("Image usage transient attachment is supported");
    }

    if (flags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
        LE_DEBUG_MESSAGE("Image usage input attachment is supported");
    }
}


static void PrintMemoryProperty(VkMemoryPropertyFlags PropertyFlags)
{
    if (PropertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
        LE_DEBUG_MESSAGE("DEVICE LOCAL");
    }

    if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        LE_DEBUG_MESSAGE("HOST VISIBLE");
    }

    if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        LE_DEBUG_MESSAGE("HOST COHERENT");
    }

    if (PropertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
        LE_DEBUG_MESSAGE("HOST CACHED");
    }

    if (PropertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) {
        LE_DEBUG_MESSAGE("LAZILY ALLOCATED");
    }

    if (PropertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT) {
        LE_DEBUG_MESSAGE("PROTECTED");
    }
}


void LeVulkanPhysicalDevices::Init(const VkInstance& Instance, const VkSurfaceKHR& Surface)
{
    uint32 NumDevices = 0;

    VkResult res = vkEnumeratePhysicalDevices(Instance, &NumDevices, NULL);
    LE_CHECK_VK_RESULT(res, "vkEnumeratePhysicalDevices error (1)");

    LE_DEBUG_MESSAGE("Num physical devices %d", NumDevices);

    m_devices.resize(NumDevices);

    std::vector<VkPhysicalDevice> Devices;
    Devices.resize(NumDevices);

    res = vkEnumeratePhysicalDevices(Instance, &NumDevices, Devices.data());
    LE_CHECK_VK_RESULT(res, "vkEnumeratePhysicalDevices error (2)");

    for (uint32 i = 0; i < NumDevices; i++)
    {
        VkPhysicalDevice PhysDev = Devices[i];
        m_devices[i].m_physDevice = PhysDev;

        vkGetPhysicalDeviceProperties(PhysDev, &m_devices[i].m_devProps);

        LE_DEBUG_MESSAGE("Device name: %s", m_devices[i].m_devProps.deviceName);
        uint32 apiVer = m_devices[i].m_devProps.apiVersion;
        LE_DEBUG_MESSAGE("    API version: %d.%d.%d.%d", 
            VK_API_VERSION_VARIANT(apiVer), 
            VK_API_VERSION_MAJOR(apiVer), 
            VK_API_VERSION_MINOR(apiVer), 
            VK_API_VERSION_PATCH(apiVer));

        uint32 NumQFamilies = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(PhysDev, &NumQFamilies, NULL);
        LE_DEBUG_MESSAGE("    Num of family queues: %d", NumQFamilies);

        m_devices[i].m_qFamilyProps.resize(NumQFamilies);
        m_devices[i].m_qSupportsPresent.resize(NumQFamilies);

        vkGetPhysicalDeviceQueueFamilyProperties(PhysDev, &NumQFamilies, m_devices[i].m_qFamilyProps.data());

        for (uint32 q = 0; q < NumQFamilies; q++) {
            const VkQueueFamilyProperties& QFamilyProp = m_devices[i].m_qFamilyProps[q];

            LE_DEBUG_MESSAGE("    Family %d Num queues: %d ", q, QFamilyProp.queueCount);
            VkQueueFlags Flags = QFamilyProp.queueFlags;
            LE_DEBUG_MESSAGE("    GFX %s, Compute %s, Transfer %s, Sparse binding %s",
                (Flags & VK_QUEUE_GRAPHICS_BIT) ? "Yes" : "No",
                (Flags & VK_QUEUE_COMPUTE_BIT) ? "Yes" : "No",
                (Flags & VK_QUEUE_TRANSFER_BIT) ? "Yes" : "No",
                (Flags & VK_QUEUE_SPARSE_BINDING_BIT) ? "Yes" : "No");

            res = vkGetPhysicalDeviceSurfaceSupportKHR(PhysDev, q, Surface, &(m_devices[i].m_qSupportsPresent[q]));
            LE_CHECK_VK_RESULT(res, "vkGetPhysicalDeviceSurfaceSupportKHR error");
        }

        uint32 NumFormats = 0;
        res = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysDev, Surface, &NumFormats, NULL);
        LE_CHECK_VK_RESULT(res, "vkGetPhysicalDeviceSurfaceFormatsKHR (1)");
        LE_DEBUG_ASSERT(NumFormats > 0);

        m_devices[i].m_surfaceFormats.resize(NumFormats);

        res = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysDev, Surface, &NumFormats, m_devices[i].m_surfaceFormats.data());
        LE_CHECK_VK_RESULT(res, "vkGetPhysicalDeviceSurfaceFormatsKHR (2)");

        for (uint32 j = 0; j < NumFormats; j++) {
            const VkSurfaceFormatKHR& SurfaceFormat = m_devices[i].m_surfaceFormats[j];
            LE_DEBUG_MESSAGE("    Format %x color space %x", SurfaceFormat.format, SurfaceFormat.colorSpace);
        }

        res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysDev, Surface, &(m_devices[i].m_surfaceCaps));
        LE_CHECK_VK_RESULT(res, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

        PrintImageUsageFlags(m_devices[i].m_surfaceCaps.supportedUsageFlags);

        uint32 NumPresentModes = 0;

        res = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysDev, Surface, &NumPresentModes, NULL);
        LE_CHECK_VK_RESULT(res, "vkGetPhysicalDeviceSurfacePresentModesKHR (1) error");

        LE_DEBUG_ASSERT(NumPresentModes != 0);

        m_devices[i].m_presentModes.resize(NumPresentModes);

        res = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysDev, Surface, &NumPresentModes, m_devices[i].m_presentModes.data());
        LE_CHECK_VK_RESULT(res, "vkGetPhysicalDeviceSurfacePresentModesKHR (2) error");

        LE_DEBUG_MESSAGE("Number of presentation modes %d", NumPresentModes);

        vkGetPhysicalDeviceMemoryProperties(PhysDev, &(m_devices[i].m_memProps));

        LE_DEBUG_MESSAGE("Num memory types %d", m_devices[i].m_memProps.memoryTypeCount);
        for (uint32 j = 0; j < m_devices[i].m_memProps.memoryTypeCount; j++) {
            LE_DEBUG_MESSAGE("%d: flags %x heap %d ", j, 
                m_devices[i].m_memProps.memoryTypes[j].propertyFlags,
                m_devices[i].m_memProps.memoryTypes[j].heapIndex);

            PrintMemoryProperty(m_devices[i].m_memProps.memoryTypes[j].propertyFlags);
        }
        LE_DEBUG_MESSAGE("Num heap types %d", m_devices[i].m_memProps.memoryHeapCount);


        vkGetPhysicalDeviceFeatures(m_devices[i].m_physDevice, &m_devices[i].m_features);

    }

    LE_DEBUG_MESSAGE("Physical devices inited");
}

void LeVulkanPhysicalDevices::DeInit()
{
    m_devices.clear();
    m_devIndex = -1;

    LE_DEBUG_MESSAGE("Physical devices cleared");
}

uint32 LeVulkanPhysicalDevices::SelectDevice(VkQueueFlags RequiredQueueType, LeBool SupportsPresent)
{
    for (uint32 i = 0; i < m_devices.size(); i++) {

        for (uint32 j = 0; j < m_devices[i].m_qFamilyProps.size(); j++) {
            const VkQueueFamilyProperties& QFamilyProp = m_devices[i].m_qFamilyProps[j];

            if ((QFamilyProp.queueFlags & RequiredQueueType) && ((LeBool)m_devices[i].m_qSupportsPresent[j] == SupportsPresent)) {
                m_devIndex = i;
                int QueueFamily = j;
                LE_DEBUG_MESSAGE("Using GFX device %d and queue family %d", m_devIndex, QueueFamily);
                return QueueFamily;
            }
        }
    }
    
    LE_ERROR("Required queue type %x and supports present %d not found", RequiredQueueType, SupportsPresent);
    
    return -1;
}


const LePhysicalDevice& LeVulkanPhysicalDevices::Selected() const
{
    if (m_devIndex < 0)
        LE_ERROR("A physical device has not been selected");

    return m_devices[m_devIndex];
}
