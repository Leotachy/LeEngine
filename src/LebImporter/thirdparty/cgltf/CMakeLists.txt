cmake_minimum_required(VERSION 3.15)
project(cgltf_proj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

  
add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE "include/")