cmake_minimum_required (VERSION 3.27)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DDT_POLYREF64=1)

project(cebreiro)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/compile_option.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/msvc.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/find_packages.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/protobuf.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(shared)
add_subdirectory(sl)
