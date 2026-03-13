
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "LeCore/LeCamera/LeCamera.h"
#include "LeCore/LeMesh/LeMeshVulkan/LeMeshVulkan.h"

constexpr int LE_MAX_FRAMES_IN_FLIGHT = 2;


void LeInitVulkan(const char* pAppName, GLFWwindow* pWindow);
void LeTerminateVulkan(void);

LeBool LeVkBeginRender(LeCamera* pCam);
void LeVkRender(LeMesh* pMesh);
void LeVkEndRender(void);



VkDevice LeGetVkLogicalDevice(void);


#define LE_CHECK_VK_RESULT(res, msg)                              \
	if (res != VK_SUCCESS)                                     \
    {                                                          \
        LE_DEBUG_ERROR("Bad vulkan result - %s, code %x", msg, res); \
	}


const LeChar* GetVkDebugUtilsMessageSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity);
const LeChar* GetVkDebugUtilsMessageType(VkDebugUtilsMessageTypeFlagsEXT Type);
uint32 GetVkBytesPerTexFormat(VkFormat Format);
