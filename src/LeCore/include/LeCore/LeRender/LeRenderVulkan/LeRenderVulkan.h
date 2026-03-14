
#pragma once

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "LeCore/LeCamera/LeCamera.h"
#include "LeCore/LeMesh/LeMesh.h"

#include "LeCore/LeRender/LeRenderVulkan/LeVkUtils.h"

constexpr int LE_MAX_FRAMES_IN_FLIGHT = 2;


void LeInitVulkan(const char* pAppName, GLFWwindow* pWindow);
void LeShutDownVulkan(void);
void LeDeInitVulkan(void);

LeBool LeVkBeginRender(LeCamera* pCam, LeBool bFramebufferResized);
LeBool LeVkRender(LeMesh* pMesh);
LeBool LeVkEndRender(void);



VkDevice LeGetVkLogicalDevice(void);


