#pragma once

#include "AST.h"
#include "symbols.h"

namespace traf {

    class ProgramBuilder : public BytecodeBuilder {
        std::vector<std::unique_ptr<Symbol>> symbols;

    public:
        using BytecodeBuilder::push;
        using BytecodeBuilder::assign;
        using BytecodeBuilder::assign_from_top;
        using BytecodeBuilder::try_delete;

        bool is_keyword(const std::string& value) const;


        ProgramBuilder() = default;

        void declare(Symbol &object) { object.declare(*this); }
        void push(const Symbol &object) { push(object.get_reference()); }
        template<typename T>
        void assign(const Symbol &dest, T &&src);
        void assign_from_top(const Symbol &dest) { assign_from_top(dest.get_reference()); }
        void try_delete(const Symbol &object) { try_delete(object.get_reference()); }
        Symbol& compile(const ASTExpression &expression, Context& parent);
        Symbol& compile(std::istream &stream, Context& parent) { return compile(*compile_expression(stream), parent); }
        Symbol& compile(std::istream &stream) { Context parent; return compile(stream, parent); }

        template<typename ... TYPES>
        Context& new_context(Context& parent, TYPES &&...args);
        template<typename T, typename ... TYPES>
        T& new_symbol(TYPES &&...args);
        Literal& new_literal(Literal literal) { return new_symbol<Literal>(std::move(literal)); }

        std::unique_ptr<ASTExpression> compile_simple_value(std::unique_ptr<ASTExpression> ast, std::istream& stream);
        std::unique_ptr<ASTExpression> compile_simple_value(std::istream& stream);
        std::unique_ptr<ASTExpression> compile_curry(std::unique_ptr<ASTExpression> ast, std::istream& stream);
        std::unique_ptr<ASTExpression> compile_curry(std::istream& stream);
        std::unique_ptr<ASTExpression> compile_value(std::istream& stream)
            { return compile_curry(stream); }
        std::unique_ptr<ASTMap> compile_map(std::istream& stream);
        std::unique_ptr<ASTExpression> compile_level1_instruction(std::unique_ptr<ASTExpression> ast, std::istream& stream);
        std::unique_ptr<ASTExpression> compile_level1_instruction(std::istream& stream);
        std::unique_ptr<ASTExpression> compile_level2_instruction(std::unique_ptr<ASTExpression> ast, std::istream& stream);
        std::unique_ptr<ASTExpression> compile_level2_instruction(std::istream &stream);
        std::unique_ptr<ASTExpression> compile_level3_instruction(std::unique_ptr<ASTExpression> ast, std::istream& stream);
        std::unique_ptr<ASTExpression> compile_level3_instruction(std::istream &stream);
        std::unique_ptr<ASTExpression> compile_level4_instruction(std::unique_ptr<ASTExpression> ast, std::istream& stream);
        std::unique_ptr<ASTExpression> compile_level4_instruction(std::istream &stream);
        std::unique_ptr<ASTExpression> compile_level5_instruction(std::unique_ptr<ASTExpression> ast, std::istream& stream);
        std::unique_ptr<ASTExpression> compile_level5_instruction(std::istream &stream);
        std::unique_ptr<ASTExpression> compile_with_instruction(std::unique_ptr<ASTExpression> ast, std::istream &stream);
        std::unique_ptr<ASTExpression> compile_with_instruction(std::istream &stream);
        std::unique_ptr<ASTExpression> compile_ifelse_instruction(std::unique_ptr<ASTExpression> ast, std::istream &stream);
        std::unique_ptr<ASTExpression> compile_ifelse_instruction(std::istream &stream);
        std::unique_ptr<ASTExpression> compile_simple_expression(std::istream& stream)
            { return compile_ifelse_instruction(stream); }
        std::unique_ptr<ASTExpression> compile_let_expression(std::istream& stream);
        std::unique_ptr<ASTExpression> compile_expression(std::istream& stream)
            { return compile_let_expression(stream); }
    };

}


template<typename T>
void traf::ProgramBuilder::assign(const Symbol &dest, T &&src) {
    if constexpr (std::is_same_v<std::remove_const_t<T>, Symbol&>)
        BytecodeBuilder::assign(dest.get_reference(), src.get_reference());
    else
        BytecodeBuilder::assign(dest.get_reference(), std::forward<T>(src));
}

template<typename T, typename ... TYPES>
T& traf::ProgramBuilder::new_symbol(TYPES &&...args) {
    static_assert(std::is_base_of_v<Symbol, T>, "New symbol must be a subclass of Symbol");
    return *reinterpret_cast<T*>(symbols.emplace_back(std::make_unique<T>(std::forward<TYPES>(args)...)).get());
}

inline std::ostream& operator<<(std::ostream& stream, const traf::Symbol& object) {
    stream << object.error_representation();
    return stream;
}





