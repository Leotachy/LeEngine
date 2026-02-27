# CMakeLists.txt for LeEngine/src/TestApp

set(_target "TestApp")
add_executable(${_target})

LeEngine_target_sources_grouped(
	${_target} TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES

	"src/main.cpp"
	"src/main.h"

	"src/window.cpp"
	"src/window.h"

)

#target_include_directories(${_target} PUBLIC "include/" PRIVATE "src/")


#file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/logs")

target_compile_definitions(${_target} 
	PUBLIC
		TESTAPP_RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/"
		TESTAPP_SHADER_BIN_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/bin/"
)

		

target_link_libraries(${_target} glfw)
target_link_libraries(${_target} LeCore)

LeEngine_target_settings(${_target} "LeEngine")