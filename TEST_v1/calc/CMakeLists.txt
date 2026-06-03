cmake_minimum_required(VERSION 3.15)

project(CALCLIB)

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${LIB_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${LIB_PATH})

add_library(${CALC_LIB} SHARED ${SRC})

target_include_directories(${CALC_LIB} PUBLIC ${HEAD_PATH})
