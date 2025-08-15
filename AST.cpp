#include "AST.h"

#include <map>

#include "ProgramBuilder.h"


using namespace project;


Symbol & ASTUnit::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return builder.new_literal(Literal::unit());
}

Symbol & ASTEvaluate::create_symbols(ProgramBuilder &builder, Context &parent) const {
    const auto symbol = parent.evaluate(name);
    if (symbol == nullptr)
        throw UndefinedSymbol(name);
    return *symbol;
}

Symbol & ASTFloat::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return builder.new_literal(Literal::floating_point(std::stod(value)));
}

Symbol & ASTInteger::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return builder.new_literal(Literal::integer(std::stoll(value)));
}

ASTLetExpression::ASTLetExpression(
    std::string variable,
    std::unique_ptr<ASTExpression> assign,
    std::unique_ptr<ASTExpression> then_do
)
    : assign(std::move(assign))
    , then_do(std::move(then_do))
    , variable(std::move(variable))
{}

Symbol & ASTLetExpression::create_symbols(ProgramBuilder &builder, Context &parent) const {
    Context context(parent);
    auto &assign_symbol = assign->create_symbols(builder, parent);
    context.new_symbol(variable, assign_symbol);
    auto &result_symbol = then_do->create_symbols(builder, context);
    return builder.new_symbol<ScopeSymbol>(assign_symbol, result_symbol);
}

Symbol & ASTBranch::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return builder.new_symbol<ConditionalResult>(
        condition->create_symbols(builder, parent),
        then_do->create_symbols(builder, parent),
        else_do->create_symbols(builder, parent)
    );
}

void ASTFunction::print(std::ostream &stream) const {
    for (const auto & parameter_name : parameter_names)
        stream << parameter_name << ' ';
    stream << ": ";
    body->print(stream);
}

Symbol & ASTFunction::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return builder.new_symbol<InlineFunction>(
        Context(parent),
        body->copy(),
        parameter_names
    );
}

void ASTMap::print(std::ostream &stream) const {
    stream << '{';
    auto first = true;
    for (const auto &[index, value] : map) {
        if (first) {
            first = false;
            stream << index << " = ";
            value->print(stream);
        }
        else {
            stream << ", " << index << " = ";
            value->print(stream);
        }
    }
    stream << '}';
}

Symbol & ASTMap::create_symbols(ProgramBuilder &builder, Context &parent) const {
    Symbol *result = &builder.new_literal(Literal::map({}));
    for (const auto &[index, value] : map)
        result = &result->set(
            Variant::integer(std::stoll(index)),
            value->create_symbols(builder, parent),
            builder
        );
    return *result;
}

std::unique_ptr<ASTExpression> ASTMap::copy() const {
    std::unordered_map<std::string, std::unique_ptr<ASTExpression>> new_map;
    for (const auto &[index, value] : map)
        new_map.emplace(index, value->copy());
    return std::make_unique<ASTMap>(std::move(new_map));
}

Symbol & ASTAddition::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return first->create_symbols(builder, parent)
        .overflow_add(second->create_symbols(builder, parent), builder);
}

Symbol & ASTDivision::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return first->create_symbols(builder, parent)
        .overflow_div(second->create_symbols(builder, parent), builder);
}

Symbol & ASTModulo::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return first->create_symbols(builder, parent)
        .overflow_mod(second->create_symbols(builder, parent), builder);
}

Symbol & ASTMultiplication::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return first->create_symbols(builder, parent)
        .overflow_mul(second->create_symbols(builder, parent), builder);
}

Symbol & ASTSubtraction::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return first->create_symbols(builder, parent)
        .overflow_sub(second->create_symbols(builder, parent), builder);
}

Symbol & ASTEquality::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return first->create_symbols(builder, parent)
        .equals(second->create_symbols(builder, parent), builder);
}

Symbol & ASTConjunction::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return first->create_symbols(builder, parent)
        .and_else(second->create_symbols(builder, parent), builder);
}

Symbol & ASTDisjunction::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return first->create_symbols(builder, parent)
        .or_else(second->create_symbols(builder, parent), builder);
}

Symbol & ASTGetIndex::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return value->create_symbols(builder, parent)
        .get(Variant::integer(std::stoll(index)), builder);
}

Symbol & ASTUpdateWith::create_symbols(ProgramBuilder &builder, Context &parent) const {
    Symbol *result = &original_value->create_symbols(builder, parent);
    for (auto &[index, value] : update_with->map ) {
        result = &result->set(
            Variant::integer(std::stoll(index)),
            value->create_symbols(builder, parent),
            builder
        );
    }
    return *result;
}

Symbol & ASTCurry::create_symbols(ProgramBuilder &builder, Context &parent) const {
    return callable->create_symbols(builder, parent)
        .curry(value->create_symbols(builder, parent), builder);
}
