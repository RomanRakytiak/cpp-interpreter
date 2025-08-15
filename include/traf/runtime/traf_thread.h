
#ifndef TRAF_THREAD_H
#define TRAF_THREAD_H

#include "runtime_basics.h"

namespace traf {

    class TrafThread {

    public:
        TrafThread();
        TrafThread(size_t uid, TrafVM& vm, word *stack, size_t stack_size, const instruction *program, size_t program_size);
        TrafThread(TrafThread&& other) noexcept;
        TrafThread(const TrafThread &) = delete;

        TrafThread &operator=(const TrafThread &) = delete;
        TrafThread &operator=(TrafThread &&other) noexcept;

        bool set_program_counter(intptr_t program_counter);
        bool move_program_counter(const intptr_t offset) { return set_program_counter(program_counter + offset); }

        bool set_stack_pointer(intptr_t stack_pointer);
        bool move_stack_pointer(const intptr_t offset) { return set_stack_pointer(stack_pointer + offset); }
        bool stack_pop() { return move_stack_pointer(-1); }

        [[nodiscard]] index stack_top() const;

        /**
         * Points at the top value.
         * @return stack top pointer
         */
        [[nodiscard]] intptr_t get_stack_pointer() const;
        [[nodiscard]] intptr_t get_program_counter() const;
        [[nodiscard]] index *get_index(intptr_t offset) const;

        template<word OP>
        bool run(word arg);
        bool run(instruction code);
        bool run(const word op, const word arg) { return run({op, arg}); }

        bool run_one_instruction();
        bool has_next_instruction();
        bool halt_program();

        void push_return_stamp();
        void backtrack_last_stump();

        bool release();
        bool is_active() const;

    private:
        void clean();

        intptr_t uid;
        TrafVM *vm;
        index *stack;
        intptr_t stack_pointer = 0;
        intptr_t stack_size;
        intptr_t last_return_stamp;
        const instruction *program;
        intptr_t program_size;
        intptr_t program_counter = 0;
    };

    template<word OP>
    bool TrafThread::run(word arg) {
    }
}

#endif //TRAF_THREAD_H
