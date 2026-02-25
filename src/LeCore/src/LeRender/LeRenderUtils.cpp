
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "Lerenderutils.h"

const char* GetVkDebugUtilsMessageSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity)
{
	switch (Severity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		return "Verbose";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		return "Info";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		return "Warning";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		return "Error";

	default:
		LE_ERROR("Invalid severity code %d\n", Severity);
	}

	return "NO SUCH SEVERITY!";
}


const char* GetVkDebugUtilsMessageType(VkDebugUtilsMessageTypeFlagsEXT Type)
{
	switch (Type) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		return "General";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		return "Validation";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		return "Performance";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
		return "Device address binding";

	default:
		LE_ERROR("Invalid type code %d\n", Type);
		exit(1);
	}

	return "NO SUCH TYPE!";
}


uint32_t GetBytesPerTexFormat(VkFormat Format)
{
	switch (Format)
	{
	case VK_FORMAT_R8_SINT:
	case VK_FORMAT_R8_UNORM:
		return 1;
	case VK_FORMAT_R16_SFLOAT:
		return 2;
	case VK_FORMAT_R16G16_SFLOAT:
		return 4;
	case VK_FORMAT_R16G16_SNORM:
		return 4;
	case VK_FORMAT_B8G8R8A8_UNORM:
		return 4;
	case VK_FORMAT_R8G8B8A8_UNORM:
		return 4;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return 4 * sizeof(uint16);
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return 4 * sizeof(float32);
	default:
		break;
	}

	return 0;
}
