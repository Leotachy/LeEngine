# CMakeLists.txt for LeEngine/src/LeCore

set(_target "LeCore")
add_library(${_target})

LeEngine_target_sources_grouped(
	${_target} TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES

	"include/LeCore/LeCore.h"

	"src/LeDebug/LeDebug.cpp"
	"src/LeDebug/LeDebugWin/DefDbg.h"

	"include/LeCore/LeEngine/LeEngine.h"
	"include/LeCore/LeRender/LeRender.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolGlobal.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolImGui.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMesh.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMaterial.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolManager.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSet.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetGlobal.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetImGui.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMesh.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMaterial.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkPhysicalDevice.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkSwapchain.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkQueue.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkUtils.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkViewport.h"
	"include/LeCore/LeRender/LeRenderVulkan/LeVkObjectCleaner.h"
	"include/LeCore/LeShader/LeShader.h"
	"include/LeCore/LeShader/LeShaderDictionary.h"
	"include/LeCore/LeShader/LeVkShader/LeVkShader.h"
	"include/LeCore/LeMesh/LeMesh.h"
	"include/LeCore/LeMesh/LeVkMesh/LeVkMesh.h"
	"include/LeCore/LeCamera/LeCamera.h"
	"include/LeCore/LeMaterial/LeMaterial.h"
	"include/LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"
	"include/LeCore/LeMaterial/LeMaterialDictionary.h"
	"include/LeCore/LeTexture/LeTexture.h"
	"include/LeCore/LeTexture/LeVkTexture/LeVkTexture.h"
	"include/LeCore/LeTexture/LeVkTexture/LeVkSamplerDictionary.h"
	"include/LeCore/LeTexture/LeTextureDictionary.h"
	"include/LeCore/LeScene/LeScene.h"
	"include/LeCore/LeWorld/LeWorld.h"
	"include/LeCore/LeStream/LeStream.h"
	"include/LeCore/LeStream/LeStreamFile.h"
	"include/LeCore/LeStore/LeStore.h"
	"include/LeCore/LeTexture/stb_image.h"
	"include/LeCore/LeUtils/LeHash.h"
	"include/LeCore/LeUtils/LeDeletionQueue.h"
	"include/LeCore/LeRGBA/LeRGBA.h"
	"include/LeCore/LeBinary/LeBinaryChunk.h"

	"src/LeEngine/LeEngine.cpp"
	"src/LeRender/LeRender.cpp"
	"src/LeRender/LeRenderVulkan/LeRenderVulkan.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPool.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolGlobal.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolImGui.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMesh.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolMaterial.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorPoolManager.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSet.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetGlobal.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetImGui.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMesh.cpp"
	"src/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetMaterial.cpp"
	"src/LeRender/LeRenderVulkan/LeVkPhysicalDevice.cpp"
	"src/LeRender/LeRenderVulkan/LeVkSwapchain.cpp"
	"src/LeRender/LeRenderVulkan/LeVkQueue.cpp"
	"src/LeRender/LeRenderVulkan/LeVkGraphicsPipeline.cpp"
	"src/LeRender/LeRenderVulkan/LeVkUtils.cpp"
	"src/LeRender/LeRenderVulkan/LeVkViewport.cpp"
	"src/LeRender/LeRenderVulkan/LeVkObjectCleaner.cpp"
	"src/LeShader/LeShader.cpp"
	"src/LeShader/LeShaderDictionary.cpp"
	"src/LeShader/LeVkShader/LeVkShader.cpp"
	"src/LeMesh/LeMesh.cpp"
	"src/LeMesh/LeVkMesh/LeVkMesh.cpp"
	"src/LeCamera/LeCamera.cpp"
	"src/LeMaterial/LeMaterial.cpp"
	"src/LeMaterial/LeVkMaterial/LeVkMaterial.cpp"
	"src/LeMaterial/LeMaterialDictionary.cpp"
	"src/LeTexture/LeTexture.cpp"
	"src/LeTexture/LeVkTexture/LeVkTexture.cpp"
	"src/LeTexture/LeVkTexture/LeVkSamplerDictionary.cpp"
	"src/LeTexture/LeTextureDictionary.cpp"
	"src/LeScene/LeScene.cpp"
	"src/LeWorld/LeWorld.cpp"
	"src/LeStream/LeStream.cpp"
	"src/LeStream/LeStreamFile.cpp"
	"src/LeStore/LeStore.cpp"
	"src/LeStore/LeStoreWin/LeStoreWin.cpp"
	"src/LeTexture/stb_image.cpp"
	"src/LeUtils/fnv64.hpp"
	"src/LeUtils/LeHash.cpp"
	"src/LeUtils/LeDeletionQueue.cpp"
	"src/LeRGBA/LeRGBA.cpp"
	"src/LeBinary/LeBinaryChunk.cpp"




)

target_include_directories(${_target} PUBLIC "include/" PRIVATE "src/")


file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/logs")

target_compile_definitions(${_target} 
	PRIVATE
		LECORE_LOG_PATH="${CMAKE_BINARY_DIR}/logs/"
)


target_link_libraries(${_target} glfw)
target_link_libraries(${_target} glm)
target_link_libraries(${_target} imgui-docking)
target_link_libraries(${_target} VulkanMemoryAllocator)

# Vulkan SDK needs to be installed on the local machine
find_package(Vulkan REQUIRED)

target_link_libraries(${_target} Vulkan::Vulkan)


LeEngine_target_settings(${_target} "LeEngine")