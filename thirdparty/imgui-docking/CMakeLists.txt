cmake_minimum_required(VERSION 3.10)
project(imgui-docking)

add_library(imgui-docking)
target_sources(imgui-docking PRIVATE 
"${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/imgui.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/imgui_demo.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/imgui_draw.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/imgui_tables.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/imgui_widgets.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends/imgui_impl_glfw.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends/imgui_impl_vulkan.cpp")

target_include_directories(imgui-docking PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking")
target_include_directories(imgui-docking PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends")

target_link_libraries(imgui-docking PUBLIC glfw)


# Vulkan SDK needs to be installed on the local machine
find_package(Vulkan REQUIRED)
target_link_libraries(imgui-docking PUBLIC Vulkan::Vulkan)