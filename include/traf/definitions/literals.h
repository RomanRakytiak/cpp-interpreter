
#ifndef LITERAL_H
#define LITERAL_H

#include "symbol.h"
#include "traf_thread.h"

template <>
struct traf::inspect_code<traf::symbols::PUSH_IMMEDIATE> {
    constexpr static size_t stack_arguments = 0;
    constexpr static size_t stack_return = 1;
    constexpr static bool has_inline_argument = true;

    static void run(TrafThread &thread, const instruction inst) {
        thread.get_index(thread.get_stack_pointer())->c_int = inst.argument;
        thread.move_stack_pointer(1);
    }

};

template <>
struct traf::symbols::inspect_symbol<traf::symbols::LITERAL_INDEX> {
    constexpr static size_t stack_returns = 1;
    constexpr static size_t stack_params = 0;

    constexpr static bool is_constexpr(index *, Interpreter &) { return true; }

    static void instantiate(index *symbol, Interpreter &interpreter, stack_address &ret) {

    }

};

#endif //LITERAL_H
