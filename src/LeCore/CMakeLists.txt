# CMakeLists.txt for LeEngine/src/LeCore

set(_target "LeCore")
add_library(${_target})

LeEngine_target_sources_grouped(
	${_target} TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES

	"include/LeCore/LeCore.h"

	"src/LeDebug/LeDebug.cpp"
	"src/LeDebug/LeDebugwin/DefDbg.h"

	"include/LeCore/LeEngine/LeEngine.h"
	"include/LeCore/LeRender/LeRender.h"
	"include/LeCore/LeRender/LeRenderResource.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"
	"include/LeCore/LeRender/LeRenderVulkan/LePhysicalDevice.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeSwapchain.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVulkanQueue.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptorSet.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkUtils.h"
	"include/LeCore/LeShader/LeShaderVulkan/LeShaderVulkan.h"
	"include/LeCore/LeShader/LeShader.h"
	"include/LeCore/LeMesh/LeMesh.h"
	"include/LeCore/LeMesh/LeMeshVulkan/LeMeshVulkan.h"
	"include/LeCore/LeCamera/LeCamera.h"
	"include/LeCore/LeMaterial/LeMaterial.h"
	"include/LeCore/LeTexture/LeTextureVulkan/LeTextureVulkan.h"
	"include/LeCore/LeTexture/LeTexture.h"
	"include/LeCore/LeTexture/stb_image.h"
	"include/LeCore/LeUtils/LeHash.h"
	"include/LeCore/LeRGBA/LeRGBA.h"

	"src/LeEngine/LeEngine.cpp"
	"src/LeRender/LeRender.cpp"
	"src/LeRender/LeRenderVulkan/LeRenderVulkan.cpp"
	"src/LeRender/LeRenderVulkan/LeVkMemoryBuffer.cpp"
	"src/LeRender/LeRenderVulkan/LePhysicalDevice.cpp"
	"src/LeRender/LeRenderVulkan/LeSwapchain.cpp"
	"src/LeRender/LeRenderVulkan/LeVulkanQueue.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptorSet.cpp"
	"src/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.cpp"
	"src/LeRender/LeRenderVulkan/LeVkUtils.cpp"
	"src/LeShader/LeShaderVulkan/LeShaderVulkan.cpp"
	"src/LeShader/LeShader.cpp"
	"src/LeMesh/LeMesh.cpp"
	"src/LeMesh/LeMeshVulkan/LeMeshVulkan.cpp"
	"src/LeCamera/LeCamera.cpp"
	"src/LeMaterial/LeMaterial.cpp"
	"src/LeTexture/LeTextureVulkan/LeTextureVulkan.cpp"
	"src/LeTexture/LeTexture.cpp"
	"src/LeTexture/stb_image.cpp"
	"src/LeUtils/fnv64.hpp"
	"src/LeUtils/LeHash.cpp"
	"src/LeRGBA/LeRGBA.cpp"




)

target_include_directories(${_target} PUBLIC "include/" PRIVATE "src/")


file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/logs")

target_compile_definitions(${_target} 
	PRIVATE
		LECORE_LOG_PATH="${CMAKE_BINARY_DIR}/logs/"
)


target_link_libraries(${_target} glfw)
target_link_libraries(${_target} glm)


# Vulkan SDK needs to be installed on the local machine
find_package(Vulkan REQUIRED)

target_link_libraries(${_target} Vulkan::Vulkan)


LeEngine_target_settings(${_target} "LeEngine")