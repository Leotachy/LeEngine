
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>


constexpr int LE_MAX_FRAMES_IN_FLIGHT = 2;


void LeVkInit(const char* pAppName, GLFWwindow* pWindow);
void LeVkTerminate();



VkDevice LeGetVkLogicalDevice(void);


#define LE_CHECK_VK_RESULT(res, msg)                              \
	if (res != VK_SUCCESS)                                     \
    {                                                          \
        LE_DEBUG_ERROR("Bad vulkan result - %s, code %x", msg, res); \
	}


const LeChar* GetVkDebugUtilsMessageSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity);
const LeChar* GetVkDebugUtilsMessageType(VkDebugUtilsMessageTypeFlagsEXT Type);
uint32 GetVkBytesPerTexFormat(VkFormat Format);
