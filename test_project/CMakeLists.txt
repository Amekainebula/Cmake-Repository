cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 17)

# set(CX main.cpp)

# set(PATH test_output)

# set(EXECUTABLE_OUTPUT_PATH ${PATH}/bin)

# aux_source_directory(${PROJECT_SOURCE_DIR} SRC)

set(OUTPUT_DIR "D:/Cmake/test_output")

# set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR}/bin)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin)

add_executable(MyProject ${SRC})






