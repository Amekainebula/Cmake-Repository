cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/*.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include) # 库文件需要头文件声明

add_executable(main ${SRC})

target_include_directories(main PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(main PUBLIC
    ${PROJECT_SOURCE_DIR}/lib1/calc.lib
)
