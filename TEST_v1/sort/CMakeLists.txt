cmake_minimum_required(VERSION 3.15)

project(SORTLIB)

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${LIB_PATH})

add_library(${SORT_LIB} STATIC ${SRC})

target_include_directories(${SORT_LIB} PUBLIC ${HEAD_PATH})