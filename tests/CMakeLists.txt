set(TEST_FILES
        TestsBytecodeBuilder.cpp
        TestsProgram.cpp
        TestsProgramBuilder.cpp
        TestsAST.cpp
        TestsSymbols.cpp
        TestsTokens.cpp
)

list(TRANSFORM TEST_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
add_library(tests OBJECT ${TEST_FILES})


set(TEST_FILES
        ${TEST_FILES}
        PARENT_SCOPE
)