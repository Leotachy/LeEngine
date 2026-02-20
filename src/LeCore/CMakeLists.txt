# CMakeLists.txt for LeEngine/src/LeCore

set(_target "LeCore")
add_executable(${_target})

LeEngine_target_sources_grouped(
	${_target} TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES

	"include/lecore/lecore.h"

	"src/ledebug/ledebug.cpp"
	"src/ledebug/ledebugwin/defdbg.h"


	"src/main.cpp"
	"src/main.h"

	
	"src/FirstApp.cpp"
	"src/FirstApp.h"

	"src/LeWindow.cpp"
	"src/LeWindow.h"

	"src/LeDevice.cpp"
	"src/LeDevice.h"

	"src/LePipeline.cpp"
	"src/LePipeline.h"

	"src/LeSwapChain.cpp"
	"src/LeSwapChain.h"

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