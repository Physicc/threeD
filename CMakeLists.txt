cmake_minimum_required(VERSION 3.10)

project(threeD)

# Set THREED_BUILD_EXAMPLES to true by default
option(THREED_BUILD_EXAMPLES "Build Examples" ON)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(threeD STATIC ${SOURCES})
target_include_directories(threeD PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# if THREED_BUILD EXAMPLES option is set
if (THREED_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()