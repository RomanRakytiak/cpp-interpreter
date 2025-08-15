set(SOURCE_FILES
        program.cpp
        bytecode_builder.cpp
        program_builder.cpp
        AST.cpp
        symbols.cpp
        core.cpp
        tokenizing.cpp
        traf_thread.cpp
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
add_library(source OBJECT ${SOURCE_FILES})

set(SOURCE_FILES
        ${SOURCE_FILES}
        PARENT_SCOPE
)