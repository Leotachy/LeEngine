
#pragma once

#include "LeCore/LeCore.h"

#define LE_CHECK_VK_RESULT(res, msg)                              \
	if (res != VK_SUCCESS)                                     \
    {                                                          \
        LE_DEBUG_ERROR("Bad vulkan result - %s, code %x", msg, res); \
	}




const char* GetVkDebugUtilsMessageSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity);

const char* GetVkDebugUtilsMessageType(VkDebugUtilsMessageTypeFlagsEXT Type);
