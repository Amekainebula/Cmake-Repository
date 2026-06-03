cmake_minimum_required(VERSION 3.15)

project(TEST_CALC)

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_PATH})

add_executable(${APP_TEST_CALC} ${SRC})

target_include_directories(${APP_TEST_CALC} PUBLIC ${HEAD_PATH})

target_link_directories(${APP_TEST_CALC} PUBLIC ${LIB_PATH})

target_link_libraries(${APP_TEST_CALC} PUBLIC ${CALC_LIB})

add_custom_command(TARGET ${APP_TEST_CALC} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${CALC_LIB}>"
        "$<TARGET_FILE_DIR:${APP_TEST_CALC}>"
)
