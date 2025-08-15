set(HEADER_FILES
        traf/bytecode_builder.h
        traf/core.h
        traf/detail/errors.h
        traf/program.h
        traf/program_builder.h
        traf/symbols.h
        traf/parsing/AST.h
        traf/parsing/lexer.h
        traf/detail/templates.h
        traf/detail/types.h
        traf/parsing/parser.h
        traf/runtime/traf_vm.h
        traf/runtime/thread_pool.h
        traf/runtime/heap_memory.h
        traf/runtime/runtime_basics.h
        traf/definitions/literals.h
        traf/definitions/symbol.h
        traf/interpreter.h
        traf/definitions/addition.h
        traf/runtime/traf_thread.h
        traf/definitions/push.h
        traf/instructions.h
        traf/definitions/architecture.h
        traf/definitions/vtable.h
)

set(HEADER_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/traf
        ${CMAKE_CURRENT_SOURCE_DIR}/traf/parsing
        ${CMAKE_CURRENT_SOURCE_DIR}/traf/definitions
        ${CMAKE_CURRENT_SOURCE_DIR}/traf/runtime
)


add_library(include INTERFACE ${HEADER_FILES})

message("header: ${HEADER_DIRECTORIES}")

target_include_directories(include INTERFACE ${HEADER_DIRECTORIES})




