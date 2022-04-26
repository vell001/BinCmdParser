cmake_minimum_required(VERSION 3.10)

set(TARGET_NAME BinCmdParser)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")

file(GLOB_RECURSE CPP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${TARGET_NAME} STATIC ${CPP_SRCS})

target_include_directories(${TARGET_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include/")