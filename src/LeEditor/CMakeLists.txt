# CMakeLists.txt for LeEngine/src/LeEditor

set(_target "LeEditor")
add_executable(${_target})

LeEngine_target_sources_grouped(
	${_target} TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES

	"include/LeEditor/LeEditor.h"
	"include/LeEditor/LeViewport.h"



	"src/LeEditor.cpp"
	"src/LeViewport.cpp"
)

target_include_directories(${_target} PUBLIC "include/" PRIVATE "src/")

target_link_libraries(${_target} LeCore)
target_link_libraries(${_target} glfw)
target_link_libraries(${_target} imgui-docking)


# Vulkan SDK needs to be installed on the local machine
find_package(Vulkan REQUIRED)
target_link_libraries(${_target} Vulkan::Vulkan)

LeEngine_target_settings(${_target} "LeEngine")