cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# add_library(calc STATIC ${SRC})
add_library(calc SHARED ${SRC})


target_include_directories(calc PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE calc)

