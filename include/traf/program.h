#pragma once

#include "core.h"

namespace traf {



    /*struct GlobalReference {
        Variant *reference;

        bool operator==(const GlobalReference&) const { return true; }
        friend std::ostream& operator<<(std::ostream& os, const GlobalReference& reference);
    };*/






    struct Program {
        using word = std::conditional_t<(sizeof(void*) > sizeof(std::uint32_t)), std::uint32_t, std::uint16_t>;
        static constexpr word max_word_limit = std::numeric_limits<word>::max();
        static constexpr word max_instructions_size = max_word_limit >> 1;
        static constexpr word max_constants_size = max_word_limit >> 1;

        enum InstructionType : word {
            OVERFLOW_ADD,
            OVERFLOW_SUB,
            OVERFLOW_MUL,
            OVERFLOW_DIV,
            OVERFLOW_MOD,
            OR_ELSE,
            AND_ELSE,
            PUSH_CONST,
            PUSH_GLOBAL,
            PUSH_STACK,
            CALL,
            POP,
            SWAP,
            PUSH_IMMEDIATE,
            DELETE,
            GET,
            SET,
            EQUAL,
            JUMP_IF_POSITIVE,
        };

        constexpr static char instr_repr[] = {
            '+', '-', '*', '/', '%',
            '|', '&',
            'c', 'g', 's',
            '@', 'p', 'w',
            'i',
            '~'
        };

        struct Instruction {
            InstructionType type;
            word argument;

            [[nodiscard]] size_t stack_arguments() const;
            [[nodiscard]] int stack_increment() const;
        };

        Program() = default;
        Program(std::vector<Instruction> instructions, std::vector<Variant> constants)
            : instructions(std::move(instructions)), constants(std::move(constants)) {}
        Program(const Program&) = default;
        Program(Program&&) = default;

        void execute(std::vector<Variant>& stack) const;
        [[nodiscard]] std::vector<Variant> execute() const;
        [[nodiscard]] Variant run() const { return execute().at(0); }
        Variant operator()() const { return run(); }

        const std::vector<Instruction> instructions;
        const std::vector<Variant> constants;

    protected:
        size_t execute_instruction(std::vector<Variant>& stack, size_t index) const;
    };

}


