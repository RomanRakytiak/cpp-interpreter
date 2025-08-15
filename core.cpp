#include "core.h"

#include <limits>
#include <strings.h>
#include "ProgramBuilder.h"

using namespace project;

Symbol * Context::evaluate(const std::string &name) const {
    if (table.contains(name) )
        return table.at(name);
    return nullptr;
}

void Context::new_symbol(const std::string &name, Symbol &symbol) {
    table[name] = &symbol;
}

InvalidSymbolAddition::InvalidSymbolAddition(const Symbol &first, const Symbol &second)
    : ProjectError("Invalid operation: " + first.error_representation() + " + " + second.error_representation())
{}

InvalidSymbolSubtraction::InvalidSymbolSubtraction(const Symbol &first, const Symbol &second)
    : ProjectError("Invalid operation: " + first.error_representation() + " - " + second.error_representation())
{}

InvalidSymbolMultiplication::InvalidSymbolMultiplication(const Symbol &first, const Symbol &second)
    : ProjectError("Invalid operation: " + first.error_representation() + " * " + second.error_representation())
{}

InvalidSymbolDivision::InvalidSymbolDivision(const Symbol &first, const Symbol &second)
    : ProjectError("Invalid operation: " + first.error_representation() + " / " + second.error_representation())
{}

InvalidSymbolModulo::InvalidSymbolModulo(const Symbol &first, const Symbol &second)
    : ProjectError("Invalid operation: " + first.error_representation() + " % " + second.error_representation())
{}

InvalidNumberOfArguments::InvalidNumberOfArguments(const Symbol &function, const size_t wants, const size_t got)
    : ProjectError("Function " + function.error_representation()
        + " expecting " + std::to_string(wants) + " arguments got instead " + std::to_string(got)) {
    assert(wants != got);
}

