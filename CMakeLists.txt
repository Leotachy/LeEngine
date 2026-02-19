# CMakeLists.txt for LeEngine

cmake_minimum_required(VERSION 3.19.2)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS        NO)

project(llengine LANGUAGES CXX)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Configure optional libraries

# Helper function to declare cache variables to control llengine options
macro (set_option variable_name help_text default_value possible_values)
    set("${variable_name}" "${default_value}" CACHE STRING "${help_text}")
    set_property(CACHE ${variable_name} PROPERTY STRINGS "${possible_values}")
endmacro ()

#set_option(SPENGINE_PHYSICS_LIBRARY   "Physics library to use with spengine. Either spdynamics or none."  	 	"none"  	"spdynamics;none"		)

#if (${SPENGINE_PHYSICS_LIBRARY} STREQUAL "spdynamics")
	#message(STATUS "spengine configured to use spdynamics for physics.")
#endif()

# end of Configure optional libraries


add_compile_definitions(
	$<$<CONFIG:Debug>:LE_DEBUG>
)


# These are in cmake/directory
include(cmake/common_options.cmake)
include(cmake/functions.cmake)

if(MSVC)
	include(cmake/msvc.cmake)
endif()




add_subdirectory(src)
add_subdirectory(thirdparty)
