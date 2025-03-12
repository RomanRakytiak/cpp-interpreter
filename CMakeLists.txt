cmake_minimum_required(VERSION 3.29)

set(TARGET_NAME cpp_project)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES
        Program.cpp
        Program.h
        BytecodeBuilder.cpp
        BytecodeBuilder.h
        ProgramBuilder.cpp
        ProgramBuilder.h
        AST.h
        AST.cpp
        core.h
        Symbols.h
        Symbols.cpp
        core.cpp
)

set(TEST_FILES
        TestsBytecodeBuilder.cpp
        TestsProgram.cpp
        TestsProgramBuilder.cpp
        TestsAST.cpp
        TestsSymbols.cpp
)

set(GTEST_SOURCE_FILES
        GoogleTest/gtest-death-test.cc
        GoogleTest/gtest-filepath.cc
        GoogleTest/gtest-port.cc
        GoogleTest/gtest-printers.cc
        GoogleTest/gtest-test-part.cc
        GoogleTest/gtest-typed-test.cc
        GoogleTest/gtest.cc
        GoogleTest/gtest_main.cc)


add_library(tests OBJECT ${TEST_FILES})
add_library(gtest OBJECT ${GTEST_SOURCE_FILES})

find_package(Threads REQUIRED)

include_directories(GoogleTest)
add_executable(${TARGET_NAME} ${SOURCE_FILES} ${TEST_FILES} ${GTEST_SOURCE_FILES}
        TestsBytecodeBuilder.cpp
        TestsProgram.cpp
        TestsProgramBuilder.cpp)
target_link_libraries(${TARGET_NAME} ${CMAKE_THREAD_LIBS_INIT})