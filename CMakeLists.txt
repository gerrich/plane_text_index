CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

include_directories(${CMAKE_INCLUDE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../gtest-svn/include/)
LINK_DIRECTORIES(${CMAKE_LIBRARY_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../gtest-svn)

add_executable(bi-stat-score bi-stat-score.cpp bi-stat-loader.cpp)
add_executable(bi-stat-build-index bi-stat-build-index.cpp bi-stat-loader.cpp)
add_executable(one-to-ne-lang-model one-to-ne-lang-model.cpp bi-stat-loader.cpp)
add_executable(check_upper_bound check_upper_bound.cpp)

enable_testing()

set(TEST_NAME binary_search_test)
set(TEST_SOURCES ${TEST_NAME}.cpp)
add_executable(${TEST_NAME} ${TEST_SOURCES})
target_link_libraries(${TEST_NAME} gtest gtest_main pthread )
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

set(TEST_NAME slice_test)
set(TEST_SOURCES ${TEST_NAME}.cpp)
add_executable(${TEST_NAME} ${TEST_SOURCES})
target_link_libraries(${TEST_NAME} gtest gtest_main pthread )
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})



