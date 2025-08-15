
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "core.h"
#include "runtime_basics.h"

namespace traf {

    alignas(index)
    struct stack_address {
        index address;
    };

    alignas(index)
    struct instruction_address {
        index address;
    };

    namespace symbols {
        class symbol;
    }

    class Interpreter {


    public:
        template<word OP, typename ...ARGS>
        void compile_op(ARGS...) {

        }

    };

}

#endif //INTERPRETER_H
