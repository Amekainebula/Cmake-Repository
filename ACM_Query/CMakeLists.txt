cmake_minimum_required(VERSION 3.15)
project(ACM_Query LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt6 MSVC 2022
set(CMAKE_PREFIX_PATH "D:/APP/Qt/6.11.1/msvc2022_64" CACHE PATH "Qt6 install prefix")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# nlohmann-json (header-only)
set(JSON_DIR D:/APP/nlohmann-json/single_include)

file(GLOB SRC src/*.cpp src/*.h)

add_executable(${PROJECT_NAME} WIN32 ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${JSON_DIR} src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    winhttp
    ws2_32
)
