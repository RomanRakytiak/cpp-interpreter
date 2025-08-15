#ifndef ADDITION_H
#define ADDITION_H

#include "literals.h"

namespace traf::defs {

    struct AddITwoInstr {
        static stack_arguments() { return 2; }
        static stack_change() { return 1; }
        void execute(TrafThread &thread, word) {
            const auto second = thread.stack_top();
            const auto first = thread.stack_top();
            thread.move_stack_pointer(-1);
            *thread.get_index(0) = index(first.c_int + second.c_int);
        }
        static bool compile(BytecodeBuilder &builder, word);
        static const char * name() { return "additwo"; }
    };

    struct AddIOneInstr {
        static stack_arguments() { return 1; }
        static stack_change() { return 1; }
        void execute(TrafThread &thread, const word arg) {
            const auto first = thread.stack_top();
            thread.move_stack_pointer(-1);
            *thread.get_index(0) = index(first.c_int + arg);
        }
        static bool compile(BytecodeBuilder &builder, word arg);
        static const char * name() { return "addione"; }
    };



}

#endif //ADDITION_H
