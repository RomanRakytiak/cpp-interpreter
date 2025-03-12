#include "ProgramBuilder.h"

#include <functional>
#include <istream>
#include <bits/ranges_algo.h>
#include <cctype>
#include <unordered_set>
#include <utility>

using namespace project;



bool ProgramBuilder::is_keyword(const std::string &value) const {
    static std::unordered_set<std::string> keywords = {
        "let", "if", "with", "else", "in"
    };
    return keywords.contains(value);
}



Symbol& ProgramBuilder::compile(const ASTExpression &expression, Context &parent) {
    auto& result = expression.create_symbols(*this, parent);
    result.declare(*this);
    result.declare_dependencies(*this);
    result.define_dependencies(*this);
    result.define(*this);
    return result;
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_simple_value(std::unique_ptr<ASTExpression> ast,
    std::istream &stream) {
    stream >> std::ws;
    switch (stream.peek()) {
        case '#': {
            stream.ignore();
            std::string second;
            stream >> second;
            if (second.empty())
                throw MissingExpression("#");
            return compile_simple_value(
                std::make_unique<ASTGetIndex>(std::move(ast), std::move(second)),
                stream
            );
        }
        default:
            return ast;
    }
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_simple_value(
    std::istream& stream
) {
    const std::streampos start_pos = stream.tellg();
    stream >> std::ws;
    switch (stream.peek()) {
        case '(': {
            stream.ignore();
            if (stream.peek() == ')') {
                stream.ignore();
                return compile_simple_value(std::make_unique<ASTUnit>(), stream);
            }
            std::unique_ptr<ASTExpression> result = compile_expression(stream);
            stream >> std::ws;
            if (stream.peek() != ')')
                throw std::runtime_error("Invalid expression missing ')'");
            stream.ignore();
            return compile_simple_value(std::move(result), stream);
        }
        case ';': {
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } break;
        case '{': {
            auto map = compile_map(stream);
            return compile_simple_value(std::move(map), stream);
        }
        default:
            break;
    }
    std::string variable;
    stream >> variable;
    if (variable.empty() || is_keyword(variable)) {
        stream.seekg(start_pos);
        return nullptr;
    }
    if (std::isdigit(variable.at(0))) {
        if (variable.contains('.'))
            return std::make_unique<ASTFloat>(std::move(variable));
        return std::make_unique<ASTInteger>(std::move(variable));
    }
    if (std::isalpha(variable.at(0)))
        return compile_simple_value(std::make_unique<ASTEvaluate>(variable), stream);
    stream.seekg(start_pos);
    return nullptr;
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_curry(std::unique_ptr<ASTExpression> ast, std::istream &stream) {
    auto argument = compile_simple_value(stream);
    if (argument == nullptr)
        return ast;
    return std::make_unique<ASTCurry>(std::move(ast), std::move(argument));
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_curry(std::istream &stream) {
    auto ast = compile_simple_value(stream);
    if (ast == nullptr)
        return nullptr;
    return compile_curry(std::move(ast), stream);
}

std::unique_ptr<ASTMap> ProgramBuilder::compile_map(std::istream &stream) {
    stream >> std::ws;
    if (stream.peek() != '{')
        return nullptr;
    stream.ignore();
    auto map = std::make_unique<ASTMap>();
    while (true) {
        stream >> std::ws;
        if (stream.peek() == '}')
            break;
        std::string index;
        stream >> index;
        if (index.empty())
            break;
        stream >> std::ws;
        if (stream.peek() != '=')
            throw MissingToken("=", index);
        stream.ignore();
        auto value = compile_expression(stream);
        if (value == nullptr)
            throw MissingExpression(index + " =");
        map->add(std::move(index), std::move(value));
        stream >> std::ws;
        if (stream.peek() != ',')
            break;
        stream.ignore();
    }
    stream >> std::ws;
    if (stream.peek() != '}')
        throw MissingToken("}", "{");
    stream.ignore();
    return std::move(map);
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_level1_instruction(
    std::unique_ptr<ASTExpression> ast,
    std::istream &stream
) {
    stream >> std::ws;
    switch (stream.peek()) {
        case '*': {
            stream.ignore();
            auto second = compile_value(stream);
            if (second == nullptr)
                throw MissingExpression("*");
            return compile_level1_instruction(
                std::make_unique<ASTMultiplication>(std::move(ast), std::move(second)),
                stream
            );
        }
        case '/': {
            stream.ignore();
            auto second = compile_value(stream);
            if (second == nullptr)
                throw MissingExpression("/");
            return compile_level1_instruction(
                std::make_unique<ASTDivision>(std::move(ast), std::move(second)),
                stream
            );
        }
        case '%': {
            stream.ignore();
            auto second = compile_value(stream);
            if (second == nullptr)
                throw MissingExpression("%");
            return compile_level1_instruction(
                std::make_unique<ASTModulo>(std::move(ast), std::move(second)),
                stream
            );
        }
        default:
            return ast;
    }
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_level1_instruction(
    std::istream &stream
) {
    auto ast = compile_value(stream);
    if (ast == nullptr)
        return nullptr;
    return compile_level1_instruction(std::move(ast),stream);
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_level2_instruction(
    std::unique_ptr<ASTExpression> ast,
    std::istream &stream
) {
    stream >> std::ws;
    switch (stream.peek()) {
        case '+': {
            stream.ignore();
            auto second = compile_level1_instruction(stream);
            if (second == nullptr)
                throw MissingExpression("+");
            return compile_level2_instruction(
                std::make_unique<ASTAddition>(std::move(ast), std::move(second)),
                stream
            );
        }
        case '-': {
            stream.ignore();
            auto second = compile_level1_instruction(stream);
            if (second == nullptr)
                throw MissingExpression("-");
            return compile_level2_instruction(
                std::make_unique<ASTSubtraction>(std::move(ast), std::move(second)),
                stream
            );
        }
        default:
            return ast;
    }
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_level2_instruction(
    std::istream &stream
) {
    auto ast = compile_level1_instruction(stream);
    if (ast == nullptr)
        return nullptr;
    return compile_level2_instruction(std::move(ast),stream);
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_level3_instruction(std::unique_ptr<ASTExpression> ast,
    std::istream &stream) {
    stream >> std::ws;
    switch (stream.peek()) {
        case '=': {
            stream.ignore();
            auto second = compile_level1_instruction(stream);
            if (second == nullptr)
                throw MissingExpression("=");
            return compile_level3_instruction(
                std::make_unique<ASTEquality>(std::move(ast), std::move(second)),
                stream
            );
        }
        default:
            return ast;
    }
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_level3_instruction(std::istream &stream) {
    auto ast = compile_level2_instruction(stream);
    if (ast == nullptr)
        return nullptr;
    return compile_level3_instruction(std::move(ast),stream);
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_level4_instruction(std::unique_ptr<ASTExpression> ast,
                                                                          std::istream &stream) {
    stream >> std::ws;
    switch (stream.peek()) {
        case '&': {
            stream.ignore();
            auto second = compile_level3_instruction(stream);
            if (second == nullptr)
                throw MissingExpression("&");
            return compile_level4_instruction(
                std::make_unique<ASTConjunction>(std::move(ast), std::move(second)),
                stream
            );
        }
        default:
            return ast;
    }
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_level4_instruction(std::istream &stream) {
    auto ast = compile_level3_instruction(stream);
    if (ast == nullptr)
        return nullptr;
    return compile_level4_instruction(std::move(ast),stream);
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_level5_instruction(std::unique_ptr<ASTExpression> ast,
                                                                          std::istream &stream) {
    stream >> std::ws;
    switch (stream.peek()) {
        case '|': {
            stream.ignore();
            auto second = compile_level4_instruction(stream);
            if (second == nullptr)
                throw MissingExpression("|");
            return compile_level5_instruction(
                std::make_unique<ASTDisjunction>(std::move(ast), std::move(second)),
                stream
            );
        }
        default:
            return ast;
    }
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_level5_instruction(std::istream &stream) {
    auto ast = compile_level4_instruction(stream);
    if (ast == nullptr)
        return nullptr;
    return compile_level5_instruction(std::move(ast),stream);
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_with_instruction(std::unique_ptr<ASTExpression> ast,
    std::istream &stream) {
    const std::streampos start_pos = stream.tellg();
    std::string keyword;
    stream >> keyword;
    if (keyword != "with") {
        stream.seekg(start_pos);
        return ast;
    }
    auto update = compile_map(stream);
    if (update == nullptr)
        throw MissingExpression("with");
    return compile_with_instruction(
        std::make_unique<ASTUpdateWith>(std::move(ast), std::move(update)),
        stream
    );
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_with_instruction(std::istream &stream) {
    auto ast = compile_level5_instruction(stream);
    if (ast == nullptr)
        return nullptr;
    return compile_with_instruction(std::move(ast),stream);
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_ifelse_instruction(
    std::unique_ptr<ASTExpression> ast,
    std::istream &stream
) {
    const std::streampos start_pos = stream.tellg();
    std::string if_keyword;
    stream >> if_keyword;
    if (if_keyword != "if") {
        stream.seekg(start_pos);
        return ast;
    }
    auto condition = compile_expression(stream);
    if (condition == nullptr)
        throw MissingExpression("... if");
    std::string else_keyword;
    stream >> else_keyword;
    if (else_keyword != "else")
        throw MissingToken("else", "... if ...");
    auto else_expression = compile_expression(stream);
    if (else_expression == nullptr)
        throw MissingExpression("... if ... else");
    return compile_ifelse_instruction(std::make_unique<ASTBranch>(
        std::move(condition),
        std::move(ast),
        std::move(else_expression)
    ), stream);
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_ifelse_instruction(std::istream &stream) {
    auto ast = compile_with_instruction(stream);
    if (ast == nullptr)
        return nullptr;
    return compile_ifelse_instruction(std::move(ast),stream);
}

std::unique_ptr<ASTExpression> ProgramBuilder::compile_let_expression(std::istream &stream) {
    const std::streampos start_pos = stream.tellg();
    std::string keyword;
    stream >> keyword;
    if (keyword != "let") {
        stream.seekg(start_pos);
        return compile_simple_expression(stream);
    }
    std::string variable_name;
    stream >> variable_name;
    char sign;
    stream >> sign;
    std::vector<std::string> parameters;
    if (sign == ':') {
        stream >> std::ws;
        while (stream.peek() != '=') {
            std::string parameter;
            stream >> parameter;
            parameters.push_back(parameter);
            stream >> std::ws;
        }
        if (parameters.empty())
            throw MissingExpression("let ... :");
        stream >> sign;
    }
    if (sign != '=')
        throw MissingToken("=", "let ...");
    auto assign = compile_expression(stream);
    if (assign == nullptr)
        throw MissingExpression("let ... =");
    stream >> keyword;
    if (keyword != "in")
        throw MissingToken("in", "let ... = ...");
    auto then_do = compile_expression(stream);
    if (then_do == nullptr)
        throw MissingExpression("let ... = ... in");
    if (parameters.empty())
        return std::make_unique<ASTLetExpression>(
            std::move(variable_name),
            std::move(assign),
            std::move(then_do)
        );
    return std::make_unique<ASTLetExpression>(
        std::move(variable_name),
     std::make_unique<ASTFunction>(std::move(assign), std::move(parameters)),
        std::move(then_do)
    );
}
