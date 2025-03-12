#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <utility>

#include "BytecodeBuilder.h"

namespace project {

    class Symbol {
    protected:
        std::optional<StackAddress> reference = std::nullopt;

    public:
        [[nodiscard]] bool is_declared() const { return reference.has_value(); }
        [[nodiscard]] StackAddress get_reference() const { return reference.value(); }

        [[nodiscard]] virtual bool is_known() const { return false; }
        [[nodiscard]] virtual bool is_trivially_destructible() const { return false; }
        [[nodiscard]] virtual std::string error_representation() const { return "<unknown>"; }

        virtual Symbol& overflow_literal_add(Literal& literal, ProgramBuilder& builder);
        virtual Symbol& overflow_literal_sub(Literal& literal, ProgramBuilder& builder);
        virtual Symbol& overflow_literal_mul(Literal& literal, ProgramBuilder& builder);
        virtual Symbol& overflow_literal_mod(Literal& literal, ProgramBuilder& builder);
        virtual Symbol& overflow_literal_div(Literal& literal, ProgramBuilder& builder);
        virtual Symbol& overflow_reverse_literal_add(Literal& literal, ProgramBuilder& builder);
        virtual Symbol& overflow_reverse_literal_sub(Literal& literal, ProgramBuilder& builder);
        virtual Symbol& overflow_reverse_literal_mul(Literal& literal, ProgramBuilder& builder);
        virtual Symbol& overflow_reverse_literal_mod(Literal& literal, ProgramBuilder& builder);
        virtual Symbol& overflow_reverse_literal_div(Literal& literal, ProgramBuilder& builder);

        virtual Symbol& overflow_add(Symbol& symbol, ProgramBuilder& builder);
        virtual Symbol& overflow_sub(Symbol& symbol, ProgramBuilder& builder);
        virtual Symbol& overflow_mul(Symbol& symbol, ProgramBuilder& builder);
        virtual Symbol& overflow_mod(Symbol& symbol, ProgramBuilder& builder);
        virtual Symbol& overflow_div(Symbol& symbol, ProgramBuilder& builder);

        virtual Symbol& and_else(Symbol& symbol, ProgramBuilder& builder) { throw ProjectError("Symbol::and_else"); }
        virtual Symbol& or_else(Symbol& symbol, ProgramBuilder& builder) { throw ProjectError("Symbol::or_else"); }
        virtual Symbol& curry(Symbol& symbol, ProgramBuilder& builder) { throw ProjectError("Symbol::curry(Symbol& symbol, ProgramBuilder& builder)"); };
        virtual Symbol& equals(Symbol& symbol, ProgramBuilder& builder) { throw ProjectError("Symbol::equals(Symbol& symbol, ProgramBuilder& builder)"); };

        virtual Symbol& set(Variant index, Symbol& value, ProgramBuilder& builder) { throw ProjectError("Symbol::set(Variant index, Symbol& value, ProgramBuilder& builder)"); };
        virtual Symbol& get(Variant index, ProgramBuilder& builder) { throw ProjectError("Symbol::get(Variant index, ProgramBuilder& builder)"); }

        virtual void declare_dependencies(ProgramBuilder& builder) {}
        virtual void declare(ProgramBuilder& builder);
        virtual void define_dependencies(ProgramBuilder& builder) {}
        virtual void define(ProgramBuilder& builder) = 0;
        virtual void delete_dependencies(ProgramBuilder& builder) {}
        virtual void destroy(ProgramBuilder& builder) {}

        virtual void push_or_define_in_place(ProgramBuilder& builder);

        virtual ~Symbol() = default;
    };

    struct ResultSymbol : Symbol {
        Symbol& overflow_literal_add(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_literal_sub(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_literal_mul(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_literal_mod(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_literal_div(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_reverse_literal_add(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_reverse_literal_sub(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_reverse_literal_mul(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_reverse_literal_mod(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_reverse_literal_div(Literal& literal, ProgramBuilder& builder) override;

        Symbol& overflow_add(Symbol& symbol, ProgramBuilder& builder) override;
        Symbol& overflow_sub(Symbol& symbol, ProgramBuilder& builder) override;
        Symbol& overflow_mul(Symbol& symbol, ProgramBuilder& builder) override;
        Symbol& overflow_mod(Symbol& symbol, ProgramBuilder& builder) override;
        Symbol& overflow_div(Symbol& symbol, ProgramBuilder& builder) override;

        Symbol& curry(Symbol& symbol, ProgramBuilder& builder) override;
        Symbol& set(Variant index, Symbol& value, ProgramBuilder& builder) override;
        Symbol& get(Variant index, ProgramBuilder& builder) override;

        Symbol& or_else(Symbol& symbol, ProgramBuilder& builder) override;
        Symbol& and_else(Symbol& symbol, ProgramBuilder& builder) override;

        void assign_or_declare_as_top(ProgramBuilder &builder);

    };

    class Literal final : public ResultSymbol {
        const Variant value;

    public:
        explicit Literal(Variant value) : value(std::move(value)) {}
        template<class T>
        static Literal integer(T value) { return Literal(Variant::integer(value)); }
        template<class T>
        static Literal floating_point(T value) { return Literal(Variant::floating_point(value)); }
        static Literal unit() { return Literal(Variant()); };
        static Literal function(Variant::function function) { return Literal(Variant(std::move(function))); }
        static Literal map(Variant::map map) { return Literal(Variant(std::move(map))); }

        [[nodiscard]] bool is_known() const override { return true; }
        [[nodiscard]] bool is_trivially_destructible() const override { return true; }

        Symbol& overflow_literal_add(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_literal_sub(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_literal_mul(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_literal_mod(Literal& literal, ProgramBuilder& builder) override;
        Symbol& overflow_literal_div(Literal& literal, ProgramBuilder& builder) override;

        Symbol& overflow_reverse_literal_add(Literal& literal, ProgramBuilder& builder) override { return literal.overflow_literal_add(*this, builder); }
        Symbol& overflow_reverse_literal_sub(Literal& literal, ProgramBuilder& builder) override { return literal.overflow_literal_sub(*this, builder); }
        Symbol& overflow_reverse_literal_mul(Literal& literal, ProgramBuilder& builder) override { return literal.overflow_literal_mul(*this, builder); }
        Symbol& overflow_reverse_literal_mod(Literal& literal, ProgramBuilder& builder) override { return literal.overflow_literal_mod(*this, builder); }
        Symbol& overflow_reverse_literal_div(Literal& literal, ProgramBuilder& builder) override { return literal.overflow_literal_div(*this, builder); }

        Symbol& overflow_add(Symbol& symbol, ProgramBuilder& builder) override { return symbol.overflow_reverse_literal_add(*this, builder); }
        Symbol& overflow_sub(Symbol& symbol, ProgramBuilder& builder) override { return symbol.overflow_reverse_literal_sub(*this, builder); }
        Symbol& overflow_mul(Symbol& symbol, ProgramBuilder& builder) override { return symbol.overflow_reverse_literal_mul(*this, builder); }
        Symbol& overflow_mod(Symbol& symbol, ProgramBuilder& builder) override { return symbol.overflow_reverse_literal_mod(*this, builder); }
        Symbol& overflow_div(Symbol& symbol, ProgramBuilder& builder) override { return symbol.overflow_reverse_literal_div(*this, builder); }

        void declare_dependencies(ProgramBuilder& builder) override {}
        void define_dependencies(ProgramBuilder& builder) override {}
        void delete_dependencies(ProgramBuilder &builder) override {}
        void destroy(ProgramBuilder& builder) override;
        void declare(ProgramBuilder& builder) override;
        void define(ProgramBuilder& builder) override;


        const Variant& operator*() const { return value; }
        const Variant* operator->() const { return &value; }

    };



    struct UpdateSymbol final : ResultSymbol {
        Symbol& value;
        std::vector<std::pair<Variant, Symbol*>> values;

        UpdateSymbol(Symbol& value, std::vector<std::pair<Variant, Symbol*>> values)
            : value(value), values(std::move(values)) {}
        explicit UpdateSymbol(Symbol& value)
            : value(value) {}

        template<std::integral T>
        void set(T index, Symbol &value) { values.emplace_back(Variant::integer(index), &value); }

        Symbol &set(Variant index, Symbol &value, ProgramBuilder &builder) override;

        void define(ProgramBuilder &builder) override;
    };

    struct GetSymbol final : ResultSymbol {
        Symbol& value;
        Variant index;

        GetSymbol(Symbol& value, Variant index) : value(value), index(std::move(index)) {}

        void define(ProgramBuilder &builder) override;
    };

    struct CurryResult final : ResultSymbol {
        Symbol& function;
        Symbol& argument;

        CurryResult(Symbol &function, Symbol &argument)
            : function(function), argument(argument) {}

        void declare_dependencies(ProgramBuilder &builder) override;
        void define_dependencies(ProgramBuilder &builder) override;
        void delete_dependencies(ProgramBuilder &builder) override;
        void define(ProgramBuilder &builder) override;
        void destroy(ProgramBuilder &builder) override {}
    };

    struct ConditionalResult : ResultSymbol {
        Symbol& condition;
        Symbol& then_do;
        Symbol& else_do;

        ConditionalResult(Symbol &condition, Symbol &then_do, Symbol &else_do)
            : condition(condition), then_do(then_do), else_do(else_do) {}

        void define(ProgramBuilder &builder) override;
        void destroy(ProgramBuilder &builder) override {}

    };

    struct FunctionSymbol : public Symbol {
        const size_t parameter_count;

        explicit FunctionSymbol(const size_t parameter_count)
            : parameter_count(parameter_count) {}

        void declare(ProgramBuilder& builder) override { throw std::logic_error("Function cannot be declared"); }
        void define(ProgramBuilder& builder) override { throw std::logic_error("Function cannot be defined");}

        [[nodiscard]] std::string error_representation() const override { return "<function>"; }
    };



    struct BinaryOperationResult : public ResultSymbol {
        enum Type {
            OVERFLOW_ADDITION = Program::OVERFLOW_ADD,
            OVERFLOW_SUBTRACTION = Program::OVERFLOW_SUB,
            OVERFLOW_MULTIPLICATION = Program::OVERFLOW_MUL,
            OVERFLOW_DIVISION = Program::OVERFLOW_DIV,
            OVERFLOW_MODULO = Program::OVERFLOW_MOD,
            EQUAL = Program::EQUAL,
            OR_ELSE = Program::OR_ELSE,
            AND_ELSE = Program::AND_ELSE,
        } const type;
        Symbol& left_operand;
        Symbol& right_operand;

        BinaryOperationResult(Type type, Symbol& left_operand, Symbol& right_operand)
            : type(type), left_operand(left_operand), right_operand(right_operand) {}

        [[nodiscard]] bool is_known() const override { return left_operand.is_known() && right_operand.is_known(); }
        [[nodiscard]] bool is_trivially_destructible() const override { return true; }

        void declare_dependencies(ProgramBuilder &builder) override;
        void define_dependencies(ProgramBuilder &builder) override;
        void define(ProgramBuilder &builder) override;
        void destroy(ProgramBuilder &builder) override;

    };

    struct InlineFunction final : public FunctionSymbol {
        Context context;
        std::unique_ptr<ASTExpression> body;
        std::vector<std::string> parameter_names;

        InlineFunction(Context context, std::unique_ptr<ASTExpression> body, std::vector<std::string>&& parameter_names)
            : FunctionSymbol(parameter_names.size()), context(std::move(context)),
            body(std::move(body)), parameter_names(std::move(parameter_names)) {}
        InlineFunction(Context context, std::unique_ptr<ASTExpression> body, const std::vector<std::string>& parameter_names)
            : InlineFunction(std::move(context), std::move(body), std::vector(parameter_names)) {}

        Symbol &curry(Symbol &symbol, ProgramBuilder &builder) override;
    };

    class FunctionResult final : public ResultSymbol {
        InlineFunction &function_symbol;
        std::vector<Symbol*> arguments;
        Symbol* result_symbol = nullptr;

    public:
        FunctionResult(InlineFunction &function_symbol, const std::vector<Symbol*>& arguments)
            : function_symbol(function_symbol), arguments(arguments) {}

        Symbol &curry(Symbol &symbol, ProgramBuilder &builder) override;
        void declare_dependencies(ProgramBuilder &builder) override;
        void declare(ProgramBuilder &builder) override;
        void define(ProgramBuilder &builder) override;

    };


}


#endif //SYMBOLS_H
