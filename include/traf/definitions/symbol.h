#ifndef SYMBOL_H
#define SYMBOL_H

#include "bytecode_builder.h"
#include "interpreter.h"

namespace traf::symbols {

    enum instruction_type {
        NOOP,
        PUSH_IMMEDIATE,
    };

    enum symbol_type {
        EMPTY_SYMBOL,
        LITERAL_INDEX,
        OVERFLOW_ADDITION,
        OVERFLOW_SUBTRACTION,
        CONDITION,
        VARIABLE,
        REFERENCE,
    };

    template<symbol_type SYMBOL>
    struct inspect_symbol;

    template<symbol_type SYMBOL>
    struct symbol_traits {
        using inspector = inspect_symbol<SYMBOL>;

        constexpr static symbol_type symbol_type = SYMBOL;
        constexpr static size_t stack_returns = inspector::stack_returns;
        constexpr static size_t stack_params = inspector::stack_params;

        static bool is_constexpr(index* symbol, Interpreter &interpreter) {
            return inspector::is_costexpr(symbol, interpreter);
        }

        template<typename... RETURNS>
        static void instantiate(index* symbol, Interpreter &interpreter, RETURNS&...args) {
            static_assert(sizeof...(RETURNS) == stack_returns, "Incorrect number of arguments");
            if constexpr (sizeof...(RETURNS) == stack_returns)
                inspector::instantiate(symbol, interpreter, args...);
        }

        template<typename... RETURNS>
        static void instantiate(index* symbol, Interpreter &interpreter, InstructionAddress &instr, RETURNS&...args) {
            static_assert(sizeof...(RETURNS) == stack_returns, "Incorrect number of arguments");
            if constexpr (sizeof...(RETURNS) == stack_returns)
                inspector::instantiate(symbol, interpreter, instr, args...);
        }
    };


    class symbol {

    public:
        [[nodiscard]] bool is_declared() const;
        [[nodiscard]] StackAddress get_reference() const;

        [[nodiscard]] virtual bool is_known() const { return false; }
        [[nodiscard]] virtual bool is_trivially_destructible() const { return false; }
        [[nodiscard]] virtual std::string error_representation() const { return "<unknown>"; }

        virtual symbol& overflow_literal_add(literal& literal, Interpreter& interpreter);
        virtual symbol& overflow_literal_sub(literal& literal, Interpreter& interpreter);
        virtual symbol& overflow_literal_mul(literal& literal, Interpreter& interpreter);
        virtual symbol& overflow_literal_mod(literal& literal, Interpreter& interpreter);
        virtual symbol& overflow_literal_div(literal& literal, Interpreter& interpreter);
        virtual symbol& overflow_reverse_literal_add(literal& literal, Interpreter& interpreter);
        virtual symbol& overflow_reverse_literal_sub(literal& literal, Interpreter& interpreter);
        virtual symbol& overflow_reverse_literal_mul(literal& literal, Interpreter& interpreter);
        virtual symbol& overflow_reverse_literal_mod(literal& literal, Interpreter& interpreter);
        virtual symbol& overflow_reverse_literal_div(literal& literal, Interpreter& interpreter);

        virtual symbol& overflow_add(symbol& symbol, Interpreter& interpreter);
        virtual symbol& overflow_sub(symbol& symbol, Interpreter& interpreter);
        virtual symbol& overflow_mul(symbol& symbol, Interpreter& interpreter);
        virtual symbol& overflow_mod(symbol& symbol, Interpreter& interpreter);
        virtual symbol& overflow_div(symbol& symbol, Interpreter& interpreter);

        virtual Symbol& and_else(Symbol& symbol, Interpreter& builder) { throw ProjectError("Symbol::and_else"); }
        virtual Symbol& or_else(Symbol& symbol, Interpreter& builder) { throw ProjectError("Symbol::or_else"); }
        virtual Symbol& curry(Symbol& symbol, Interpreter& builder) { throw ProjectError("Symbol::curry(Symbol& symbol, ProgramBuilder& builder)"); };
        virtual Symbol& equals(Symbol& symbol, Interpreter& builder) { throw ProjectError("Symbol::equals(Symbol& symbol, ProgramBuilder& builder)"); };

        virtual Symbol& set(Variant index, Symbol& value, Interpreter& builder) { throw ProjectError("Symbol::set(Variant index, Symbol& value, ProgramBuilder& builder)"); };
        virtual Symbol& get(Variant index, Interpreter& builder) { throw ProjectError("Symbol::get(Variant index, ProgramBuilder& builder)"); }

        virtual void declare_dependencies(ProgramBuilder& builder) {}
        virtual void declare(ProgramBuilder& builder);
        virtual void define_dependencies(ProgramBuilder& builder) {}
        virtual void define(ProgramBuilder& builder) = 0;
        virtual void delete_dependencies(ProgramBuilder& builder) {}
        virtual void destroy(ProgramBuilder& builder) {}

        virtual bool needs_defining() { return true; }

        virtual void push_or_define_in_place(ProgramBuilder& builder);

        virtual ~Symbol() = default;
    };

    struct pushable_symbol : symbol {

        virtual ~trait_pushable() = default;
    };

    struct evaluable_symbol : symbol {
        virtual evaluable_symbol& const_overflow_add(evaluable_symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_sub(evaluable_symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_mul(evaluable_symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_mod(evaluable_symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_div(evaluable_symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_reverse_add(evaluable_symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_reverse_sub(evaluable_symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_reverse_mul(evaluable_symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_reverse_mod(evaluable_symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_reverse_div(evaluable_symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_add(symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_sub(symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_mul(symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_mod(symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_div(symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_reverse_add(symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_reverse_sub(symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_reverse_mul(symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_reverse_mod(symbol& literal, Interpreter& interpreter);
        virtual evaluable_symbol& const_overflow_reverse_div(symbol& literal, Interpreter& interpreter);

        symbol& overflow_add(symbol& other, Interpreter& interpreter) override;
        symbol& overflow_sub(symbol& other, Interpreter& interpreter) override;
        symbol& overflow_mul(symbol& other, Interpreter& interpreter) override;
        symbol& overflow_mod(symbol& other, Interpreter& interpreter) override;
        symbol& overflow_div(symbol& other, Interpreter& interpreter) override;

        ~evaluable_symbol() override = default;
    };
}

#endif //SYMBOL_H
