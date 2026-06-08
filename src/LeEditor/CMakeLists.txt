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

target_link_libraries(${_target} PRIVATE LeCore)

LeEngine_target_settings(${_target} "LeEngine")