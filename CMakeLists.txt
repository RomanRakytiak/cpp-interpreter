cmake_minimum_required(VERSION 3.29)

set(TARGET_NAME cpp_project)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES)
set(HEADER_FILES)
set(TEST_FILES)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(tests)

message("source: ${SOURCE_FILES}")

set(GTEST_SOURCE_FILES
        GoogleTest/gtest-death-test.cc
        GoogleTest/gtest-filepath.cc
        GoogleTest/gtest-port.cc
        GoogleTest/gtest-printers.cc
        GoogleTest/gtest-test-part.cc
        GoogleTest/gtest-typed-test.cc
        GoogleTest/gtest.cc
        GoogleTest/gtest_main.cc
)

add_library(gtest OBJECT ${GTEST_SOURCE_FILES})


find_package(Threads REQUIRED)

include_directories(GoogleTest)
add_executable(${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${TEST_FILES} ${GTEST_SOURCE_FILES})
target_link_libraries(${TARGET_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${TARGET_NAME} PRIVATE include)