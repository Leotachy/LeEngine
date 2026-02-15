# CMakeLists.txt for LeEngine/src/LeCore

set(_target "LeCore")
add_executable(${_target})

LeEngine_target_sources_grouped(
	${_target} TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES

	src/main.cpp
	src/main.h

	src/camera.h

	src/stb_image.cpp
	src/stb_image.h

)

target_compile_definitions(${_target} PUBLIC LECORE_RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")


target_link_libraries(${_target} glfw)
target_link_libraries(${_target} glm)


# Vulkan SDK needs to be installed on the local machine
find_package(Vulkan REQUIRED)

target_link_libraries(${_target} Vulkan::Vulkan)


LeEngine_target_settings(${_target} "LeEngine")