# CMakeLists.txt for LeEngine/src/TestApp

set(_target "TestApp")
add_executable(${_target})

LeEngine_target_sources_grouped(
	${_target} TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES

	"src/main.cpp"

)

#target_include_directories(${_target} PUBLIC "include/" PRIVATE "src/")


#file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/logs")

target_compile_definitions(${_target} 
	PUBLIC
		TESTAPP_RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/"
		TESTAPP_SHADER_BIN_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/bin/"
		TESTAPP_SCENES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/scenes/"
)

		

target_link_libraries(${_target} PRIVATE LeCore)
target_link_libraries(${_target} PRIVATE LebImporter)

LeEngine_target_settings(${_target} "LeEngine")