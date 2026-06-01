cmake_minimum_required(VERSION 3.15)

project(MyProject)

set(CMAKE_CXX_STANDARD 20)

set(a a123)
set(b b456)
set(c c789)

list(APPEND d ${a} ${b} ${c})

message(STATUS ${d})

file(GLOB SRC ${PROJECT_SOURCE_DIR}/*.cpp)

message("ok")

add_executable(main ${SRC})

# include_directories(${PROJECT_SOURCE_DIR}/include) # 库文件需要头文件声明

target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(main PUBLIC ${PROJECT_SOURCE_DIR}/lib2/calc.lib)

message(STATUS "finish")

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${PROJECT_SOURCE_DIR}/lib2/calc.dll
            $<TARGET_FILE_DIR:main>
)
