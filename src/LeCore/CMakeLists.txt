# CMakeLists.txt for LeEngine/src/LeCore

set(_target "LeCore")
add_library(${_target})

LeEngine_target_sources_grouped(
	${_target} TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES

	"include/LeCore/LeCore.h"

	"src/LeDebug/LeDebug.cpp"
	"src/LeDebug/LeDebugwin/DefDbg.h"

	"include/LeCore/LeRender/LeDevice.h"
	"include/LeCore/LeRender/LeRender.h"
	"include/LeCore/LeRender/LeVulkanQueue.h"
	"src/LeRender/LeDevice.cpp"
	"src/LeRender/LeRender.cpp"
	"src/LeRender/LeVulkanQueue.cpp"
	"src/LeRender/LeRenderUtils.cpp"
	"src/LeRender/LeRenderUtils.h"

	"src/camera.h"
	"src/stb_image.cpp"
	"src/stb_image.h"



)

target_include_directories(${_target} PUBLIC "include/" PRIVATE "src/")


file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/logs")

target_compile_definitions(${_target} 
	PUBLIC
		LECORE_RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/"
	PRIVATE
		LECORE_LOG_PATH="${CMAKE_BINARY_DIR}/logs/"
)


target_link_libraries(${_target} glfw)
target_link_libraries(${_target} glm)


# Vulkan SDK needs to be installed on the local machine
find_package(Vulkan REQUIRED)

target_link_libraries(${_target} Vulkan::Vulkan)


LeEngine_target_settings(${_target} "LeEngine")