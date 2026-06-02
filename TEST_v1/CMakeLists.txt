cmake_minimum_required(VERSION 3.15)
project(test)

set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(EXEC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(HEAD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CALC_LIB calc)

set(SORT_LIB sort)

set(APP_TEST_CALC test_calc)

set(APP_TEST_SORT test_sort)

add_subdirectory(calc)

add_subdirectory(sort)

add_subdirectory(test_calc)

add_subdirectory(test_sort)