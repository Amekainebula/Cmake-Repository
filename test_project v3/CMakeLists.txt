cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/*.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include) # 库文件需要头文件声明

# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# add_library(calc STATIC ${SRC})
# add_library(calc SHARED ${SRC})

link_directories(${PROJECT_SOURCE_DIR}/lib1) # 非系统库需要指定路径

link_libraries(calc)

add_executable(main ${SRC})

