# CMakeLists.txt for LeEngine/src/LebImporter

set(_target "LebImporter")
add_library(${_target})

LeEngine_target_sources_grouped(
	${_target} TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES

	"include/LebImporter/LebImporter.h"

	"src/LebImporter.cpp"
)

target_include_directories(${_target} PUBLIC "include/" PRIVATE "src/")

target_link_libraries(${_target} PUBLIC LeCore)

find_package(cgltf REQUIRED CONFIG)
target_link_libraries(${_target} PRIVATE cgltf::cgltf)

LeEngine_target_settings(${_target} "LeEngine")