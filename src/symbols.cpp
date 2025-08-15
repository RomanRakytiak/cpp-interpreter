#include "symbols.h"

//#include <bits/locale_facets_nonio.h>
#include <functional>


#include "program_builder.h"

using namespace traf;



Symbol & Symbol::overflow_literal_add(Literal &literal, ProgramBuilder &builder) {
    throw InvalidSymbolAddition(*this, literal);
}

Symbol & Symbol::overflow_literal_sub(Literal &literal, ProgramBuilder &builder) {
    throw InvalidSymbolSubtraction(*this, literal);
}

Symbol & Symbol::overflow_literal_mul(Literal &literal, ProgramBuilder &builder) {
    throw InvalidSymbolMultiplication(*this, literal);
}

Symbol & Symbol::overflow_literal_mod(Literal &literal, ProgramBuilder &builder) {
    throw InvalidSymbolModulo(*this, literal);
}

Symbol & Symbol::overflow_literal_div(Literal &literal, ProgramBuilder &builder) {
    throw InvalidSymbolDivision(*this, literal);
}

Symbol & Symbol::overflow_reverse_literal_add(Literal &literal, ProgramBuilder &builder) {
    throw InvalidSymbolAddition(literal, *this);
}

Symbol & Symbol::overflow_reverse_literal_sub(Literal &literal, ProgramBuilder &builder) {
    throw InvalidSymbolSubtraction(literal, *this);
}

Symbol & Symbol::overflow_reverse_literal_mul(Literal &literal, ProgramBuilder &builder) {
    throw InvalidSymbolMultiplication(literal, *this);
}

Symbol & Symbol::overflow_reverse_literal_mod(Literal &literal, ProgramBuilder &builder) {
    throw InvalidSymbolModulo(literal, *this);
}

Symbol & Symbol::overflow_reverse_literal_div(Literal &literal, ProgramBuilder &builder) {
    throw InvalidSymbolDivision(literal, *this);
}

Symbol & Symbol::overflow_add(Symbol &symbol, ProgramBuilder &builder) {
    if (const auto literal = dynamic_cast<Literal *>(&symbol))
        return overflow_literal_add(*literal, builder);
    throw InvalidSymbolAddition(symbol, *this);
}

Symbol & Symbol::overflow_sub(Symbol &symbol, ProgramBuilder &builder) {
    if (const auto literal = dynamic_cast<Literal *>(&symbol))
        return overflow_literal_sub(*literal, builder);
    throw InvalidSymbolSubtraction(symbol, *this);
}

Symbol & Symbol::overflow_mul(Symbol &symbol, ProgramBuilder &builder) {
    if (const auto literal = dynamic_cast<Literal *>(&symbol))
        return overflow_literal_mul(*literal, builder);
    throw InvalidSymbolMultiplication(symbol, *this);
}

Symbol & Symbol::overflow_mod(Symbol &symbol, ProgramBuilder &builder) {
    if (const auto literal = dynamic_cast<Literal *>(&symbol))
        return overflow_literal_mod(*literal, builder);
    throw InvalidSymbolModulo(symbol, *this);
}

Symbol & Symbol::overflow_div(Symbol &symbol, ProgramBuilder &builder) {
    if (const auto literal = dynamic_cast<Literal *>(&symbol))
        return overflow_literal_div(*literal, builder);
    throw InvalidSymbolDivision(symbol, *this);
}

void Symbol::declare(ProgramBuilder &builder) {
    assert(is_declared() == false);
    reference.emplace(builder.push_unit());
}

void Symbol::push_or_define_in_place(ProgramBuilder &builder) {
    if (is_declared())
        builder.push(*reference);
    else
        define(builder);
}

Symbol & ResultSymbol::overflow_literal_add(Literal &literal, ProgramBuilder &builder) {
    return overflow_add(literal, builder);
}

Symbol & ResultSymbol::overflow_literal_sub(Literal &literal, ProgramBuilder &builder) {
    return overflow_sub(literal, builder);
}

Symbol & ResultSymbol::overflow_literal_mul(Literal &literal, ProgramBuilder &builder) {
    return overflow_mul(literal, builder);
}

Symbol & ResultSymbol::overflow_literal_mod(Literal &literal, ProgramBuilder &builder) {
    return overflow_mod(literal, builder);
}

Symbol & ResultSymbol::overflow_literal_div(Literal &literal, ProgramBuilder &builder) {
    return overflow_div(literal, builder);
}

Symbol & ResultSymbol::overflow_reverse_literal_add(Literal &literal, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(
        BinaryOperationResult::OVERFLOW_ADDITION,
        literal,
        *this
    );
}

Symbol & ResultSymbol::overflow_reverse_literal_sub(Literal &literal, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(
        BinaryOperationResult::OVERFLOW_SUBTRACTION,
        literal,
        *this
    );
}

Symbol & ResultSymbol::overflow_reverse_literal_mul(Literal &literal, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(
        BinaryOperationResult::OVERFLOW_MULTIPLICATION,
        literal,
        *this
    );
}

Symbol & ResultSymbol::overflow_reverse_literal_mod(Literal &literal, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(
        BinaryOperationResult::OVERFLOW_MODULO,
        literal,
        *this
    );
}

Symbol & ResultSymbol::overflow_reverse_literal_div(Literal &literal, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(
        BinaryOperationResult::OVERFLOW_DIVISION,
        literal,
        *this
    );
}

Symbol & ResultSymbol::overflow_add(Symbol &symbol, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(
        BinaryOperationResult::OVERFLOW_ADDITION,
        *this,
        symbol
    );
}

Symbol & ResultSymbol::overflow_sub(Symbol &symbol, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(
        BinaryOperationResult::OVERFLOW_SUBTRACTION,
        *this,
        symbol
    );
}

Symbol & ResultSymbol::overflow_mul(Symbol &symbol, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(
        BinaryOperationResult::OVERFLOW_MULTIPLICATION,
        *this,
        symbol
    );
}

Symbol & ResultSymbol::overflow_mod(Symbol &symbol, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(
        BinaryOperationResult::OVERFLOW_MODULO,
        *this,
        symbol
    );
}

Symbol & ResultSymbol::overflow_div(Symbol &symbol, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(
        BinaryOperationResult::OVERFLOW_DIVISION,
        *this,
        symbol
    );
}

Symbol & ResultSymbol::curry(Symbol &symbol, ProgramBuilder &builder) {
    return builder.new_symbol<CurryResult>(*this, symbol);
}

Symbol & ResultSymbol::set(Variant index, Symbol &value, ProgramBuilder &builder) {
    return builder.new_symbol<UpdateSymbol>(*this).set(index, value, builder);
}

Symbol & ResultSymbol::get(Variant index, ProgramBuilder &builder) {
    return builder.new_symbol<GetSymbol>(*this, std::move(index));
}

Symbol & ResultSymbol::or_else(Symbol &symbol, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(BinaryOperationResult::OR_ELSE, *this, symbol);
}

Symbol & ResultSymbol::and_else(Symbol &symbol, ProgramBuilder &builder) {
    return builder.new_symbol<BinaryOperationResult>(BinaryOperationResult::AND_ELSE, *this, symbol);
}

void ResultSymbol::assign_or_declare_as_top(ProgramBuilder &builder) {
    if (is_declared())
        builder.assign_from_top(*reference);
    else
        reference.emplace(builder.stack_top());
}

void ScopeSymbol::define(ProgramBuilder &builder) {
    if (is_declared() == false)
        declare(builder);
    auto frame = builder.new_stack_frame();
    for (const auto &var : variables) {
        if (var->needs_defining())
            var->define(builder);
    }
    expression.push_or_define_in_place(builder);
    assign_or_declare_as_top(builder);
    frame.end_frame();
}

Symbol & UpdateSymbol::set(Variant index, Symbol &value, ProgramBuilder &builder) {
    values.emplace_back(index, &value);
    return *this;
}

void UpdateSymbol::define(ProgramBuilder &builder) {
    value.push_or_define_in_place(builder);
    for (auto &[index, value]: values) {
        builder.push(index);
        value->push_or_define_in_place(builder);
        builder.command(Program::SET);
    }

    builder.assign_from_top(value);
}

void GetSymbol::define(ProgramBuilder &builder) {
    value.push_or_define_in_place(builder);
    builder.push(index);
    builder.command(Program::GET);
}

void CurryResult::declare_dependencies(ProgramBuilder &builder) {
    function.declare_dependencies(builder);
    argument.declare_dependencies(builder);
}

void CurryResult::define_dependencies(ProgramBuilder &builder) {
    function.define_dependencies(builder);
    argument.define_dependencies(builder);
}

void CurryResult::delete_dependencies(ProgramBuilder &builder) {
    function.delete_dependencies(builder);
    argument.delete_dependencies(builder);
}

void CurryResult::define(ProgramBuilder &builder) {
    argument.push_or_define_in_place(builder);
    function.push_or_define_in_place(builder);
    builder.command({Program::CALL, 1});
    assign_or_declare_as_top(builder);
}

void ConditionalResult::define(ProgramBuilder &builder) {
    condition.push_or_define_in_place(builder);
    const auto condition_jump = builder.jump_if_stack_top_positive();
    builder.virtual_push();

    auto else_frame = builder.new_stack_frame();
    else_frame.pop_variables = false;
    else_do.push_or_define_in_place(builder);
    const auto else_jump = builder.unconditional_jump();
    else_frame.end_frame();

    auto then_frame = builder.new_stack_frame();
    then_frame.pop_variables = false;
    const auto then_start = builder.next_instruction_address();
    then_do.push_or_define_in_place(builder);
    const auto then_end = builder.next_instruction_address();
    then_frame.end_frame();

    builder.update_jump_location(condition_jump, then_start);
    builder.update_jump_location(else_jump, then_end);
    assign_or_declare_as_top(builder);
}

Symbol & InlineFunction::curry(Symbol &symbol, ProgramBuilder &builder) {
    return builder.new_symbol<FunctionResult>(FunctionResult(*this, {&symbol}));
}

Symbol & FunctionResult::curry(Symbol &symbol, ProgramBuilder &builder) {
    if (arguments.size() == function_symbol.parameter_count)
        return ResultSymbol::curry(symbol, builder);
    auto &result = builder.new_symbol<FunctionResult>(*this);
    result.arguments.push_back(&symbol);
    return result;
}

void FunctionResult::declare_dependencies(ProgramBuilder &builder) {
    for (auto &argument : arguments) {
        if (argument->is_declared() == false) {
            argument->declare_dependencies(builder);
            argument->declare(builder);
        }
    }
}

void FunctionResult::declare(ProgramBuilder &builder) {
    ResultSymbol::declare(builder);
}

void FunctionResult::define(ProgramBuilder &builder) {
    if (arguments.size() != function_symbol.parameter_count)
        throw InvalidNumberOfArguments(function_symbol, arguments.size(), arguments.size() + 1);
    auto context = function_symbol.context;
    for (int i = 0; i < arguments.size(); ++i)
        context.new_symbol(function_symbol.parameter_names.at(i), *arguments.at(i));
    result_symbol = &function_symbol.body->create_symbols(builder, context);
    result_symbol->push_or_define_in_place(builder);
    assign_or_declare_as_top(builder);
}


Symbol & Literal::overflow_literal_add(Literal &literal, ProgramBuilder &builder) {
    auto result = value.overflow_add(literal.value);
    if (result.has_value() == false)
        throw InvalidSymbolAddition(literal, *this);
    return builder.new_symbol<Literal>(result.value());
}

Symbol & Literal::overflow_literal_sub(Literal &literal, ProgramBuilder &builder) {
    auto result = value.overflow_sub(literal.value);
    if (result.has_value() == false)
        throw InvalidSymbolSubtraction(literal, *this);
    return builder.new_symbol<Literal>(result.value());
}

Symbol & Literal::overflow_literal_mul(Literal &literal, ProgramBuilder &builder) {
    auto result = value.overflow_mul(literal.value);
    if (result.has_value() == false)
        throw InvalidSymbolMultiplication(literal, *this);
    return builder.new_symbol<Literal>(result.value());
}

Symbol & Literal::overflow_literal_div(Literal &literal, ProgramBuilder &builder) {
    auto result = value.overflow_div(literal.value);
    if (result.has_value() == false)
        throw InvalidSymbolDivision(literal, *this);
    return builder.new_symbol<Literal>(result.value());
}

Symbol & Literal::overflow_literal_mod(Literal &literal, ProgramBuilder &builder) {
    auto result = value.overflow_mod(literal.value);
    if (result.has_value() == false)
        throw InvalidSymbolModulo(literal, *this);
    return builder.new_symbol<Literal>(result.value());
}

void Literal::destroy(ProgramBuilder &builder) {
    builder.assign(reference.value(), Variant());
}

void Literal::declare(ProgramBuilder &builder) {
    assert(is_declared() == false);
    reference.emplace(builder.push(value));
}

void Literal::define(ProgramBuilder &builder) {
    if (reference.has_value() == false)
        reference.emplace(builder.push(value));
}

void BinaryOperationResult::declare_dependencies(ProgramBuilder &builder) {
    left_operand.declare_dependencies(builder);
    right_operand.declare_dependencies(builder);
}

void BinaryOperationResult::define_dependencies(ProgramBuilder &builder) {
    left_operand.define_dependencies(builder);
    right_operand.define_dependencies(builder);
}

void BinaryOperationResult::define(ProgramBuilder &builder) {
    left_operand.push_or_define_in_place(builder);
    right_operand.push_or_define_in_place(builder);
    builder.command(static_cast<BytecodeBuilder::Instruction>(type));
    assign_or_declare_as_top(builder);
}

void BinaryOperationResult::destroy(ProgramBuilder &builder) {
    if (is_trivially_destructible() == false)
        builder.try_delete(get_reference());
}

