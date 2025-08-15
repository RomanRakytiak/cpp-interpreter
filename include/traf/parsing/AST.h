#ifndef AST_H
#define AST_H

#include "bytecode_builder.h"
#include "lexer.h"

namespace traf {
    struct ASTExpression {
        using Instruction = BytecodeBuilder::Instruction;
        virtual ~ASTExpression() = default;
        virtual Symbol& create_symbols(ProgramBuilder& builder, Context& parent) const = 0;

        friend std::ostream& operator<<(std::ostream& stream, const ASTExpression& expression) {
            expression.print(stream);
            return stream;
        }

        virtual void print(std::ostream& stream) const = 0;
        virtual std::unique_ptr<ASTExpression> copy() const = 0;
    };

    struct ASTValue : ASTExpression {
        std::vector<token> tokens;
        explicit ASTValue(std::vector<token> tokens) : tokens(std::move(tokens)) {}
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        void print(std::ostream& stream) const override { stream << value; }
        std::unique_ptr<ASTExpression> copy() const override { return std::make_unique<ASTFloat>(value); }
    };

    struct ASTFloat final : ASTExpression {
        std::string value;
        explicit ASTFloat(std::string value) : value(std::move(value)) {}
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        void print(std::ostream& stream) const override { stream << value; }
        std::unique_ptr<ASTExpression> copy() const override { return std::make_unique<ASTFloat>(value); }
    };

    struct ASTInteger final : ASTExpression  {
        std::string value;
        explicit ASTInteger(std::string value) : value(std::move(value)) {}
        Symbol & create_symbols(ProgramBuilder &builder, Context &parent) const override;
        void print(std::ostream& stream) const override { stream << value; }
        std::unique_ptr<ASTExpression> copy() const override { return std::make_unique<ASTInteger>(value); }
    };

    struct ASTUnit final : ASTExpression  {
        Symbol & create_symbols(ProgramBuilder &builder, Context &parent) const override;
        void print(std::ostream& stream) const override { stream << Unit(); }
        std::unique_ptr<ASTExpression> copy() const override { return std::make_unique<ASTUnit>(); }
    };

    struct ASTEvaluate final : ASTExpression  {
        std::string name;
        explicit ASTEvaluate(std::string name) : name(std::move(name)) {}
        Symbol & create_symbols(ProgramBuilder &builder, Context &parent) const override;
        void print(std::ostream& stream) const override { stream << name; }
        std::unique_ptr<ASTExpression> copy() const override { return std::make_unique<ASTEvaluate>(name); }
    };

    struct ASTLetExpression final : ASTExpression {
        std::unique_ptr<ASTExpression> assign;
        std::unique_ptr<ASTExpression> then_do;
        std::string variable;
        ASTLetExpression(std::string variable, std::unique_ptr<ASTExpression> assign, std::unique_ptr<ASTExpression> then_do);
        void print(std::ostream& stream) const override
        { stream << "let " << variable << " = " <<  *assign << "\nin " << *then_do; }

        Symbol & create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTLetExpression>(variable, assign->copy(), then_do->copy()); }
    };

    struct ASTBranch final : ASTExpression {
        std::unique_ptr<ASTExpression> condition;
        std::unique_ptr<ASTExpression> then_do;
        std::unique_ptr<ASTExpression> else_do;
        ASTBranch(std::unique_ptr<ASTExpression> condition, std::unique_ptr<ASTExpression> then_do, std::unique_ptr<ASTExpression> else_do)
            : condition(std::move(condition)), then_do(std::move(then_do)), else_do(std::move(else_do)) {}
        void print(std::ostream& stream) const override
        { stream << *then_do << " if " << *condition << " else " << *else_do; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTBranch>(condition->copy(), then_do->copy(), else_do->copy()); ;}
    };

    struct ASTFunction final : ASTExpression {
        std::unique_ptr<ASTExpression> body;
        std::vector<std::string> parameter_names;
        ASTFunction(std::unique_ptr<ASTExpression> body, std::vector<std::string> parameter_names)
            : body(std::move(body)), parameter_names(std::move(parameter_names)) {}
        void print(std::ostream& stream) const override;
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTFunction>(body->copy(), parameter_names); }
    };

    struct ASTMap final : ASTExpression {
        std::unordered_map<std::string, std::unique_ptr<ASTExpression>> map;
        ASTMap() = default;
        explicit ASTMap(std::unordered_map<std::string, std::unique_ptr<ASTExpression>> map)
            : map(std::move(map)) {}
        void print(std::ostream& stream) const override;
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        void add(std::string name, std::unique_ptr<ASTExpression> expression)
        { map.emplace(std::move(name), std::move(expression)); }
        std::unique_ptr<ASTExpression> copy() const override;
    };

    struct ASTAddition final : ASTExpression {
        std::unique_ptr<ASTExpression> first;
        std::unique_ptr<ASTExpression> second;
        ASTAddition(std::unique_ptr<ASTExpression> first, std::unique_ptr<ASTExpression> second)
          : first(std::move(first)), second(std::move(second)) {}
        void print(std::ostream& stream) const override { stream << *first << " + " << *second; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTAddition>(first->copy(), second->copy()); }
    };

    struct ASTDivision final : ASTExpression {
        std::unique_ptr<ASTExpression> first;
        std::unique_ptr<ASTExpression> second;
        ASTDivision(std::unique_ptr<ASTExpression> first, std::unique_ptr<ASTExpression> second)
          : first(std::move(first)), second(std::move(second)) {}
        void print(std::ostream& stream) const override { stream << *first << " / " << *second; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTDivision>(first->copy(), second->copy()); }
    };

    struct ASTModulo final : ASTExpression {
        std::unique_ptr<ASTExpression> first;
        std::unique_ptr<ASTExpression> second;
        ASTModulo(std::unique_ptr<ASTExpression> first, std::unique_ptr<ASTExpression> second)
          : first(std::move(first)), second(std::move(second)) {}
        void print(std::ostream& stream) const override { stream << *first << " % " << *second; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTModulo>(first->copy(), second->copy()); }

    };

    struct ASTMultiplication final : ASTExpression {
        std::unique_ptr<ASTExpression> first;
        std::unique_ptr<ASTExpression> second;
        ASTMultiplication(std::unique_ptr<ASTExpression> first, std::unique_ptr<ASTExpression> second)
          : first(std::move(first)), second(std::move(second)) {}
        void print(std::ostream& stream) const override { stream << *first << " * " << *second; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTMultiplication>(first->copy(), second->copy()); }

    };

    struct ASTSubtraction final : ASTExpression {
        std::unique_ptr<ASTExpression> first;
        std::unique_ptr<ASTExpression> second;
        ASTSubtraction(std::unique_ptr<ASTExpression> first, std::unique_ptr<ASTExpression> second)
          : first(std::move(first)), second(std::move(second)) {}
        void print(std::ostream& stream) const override { stream << *first << " - " << *second; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTSubtraction>(first->copy(), second->copy()); }

    };

    struct ASTNegation final : ASTExpression {
        std::unique_ptr<ASTExpression> value;
        explicit ASTNegation(std::unique_ptr<ASTExpression> value) : value(std::move(value)) {}
        void print(std::ostream& stream) const override { stream << '-' << *value; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTNegation>(value->copy()); }
    };

    struct ASTEquality final : ASTExpression {
        std::unique_ptr<ASTExpression> first;
        std::unique_ptr<ASTExpression> second;
        ASTEquality(std::unique_ptr<ASTExpression> first, std::unique_ptr<ASTExpression> second)
           : first(std::move(first)), second(std::move(second)) {}
        void print(std::ostream& stream) const override { stream << *first << " = " << *second; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTEquality>(first->copy(), second->copy()); }
    };

    struct ASTConjunction final : ASTExpression {
        std::unique_ptr<ASTExpression> first;
        std::unique_ptr<ASTExpression> second;
        ASTConjunction(std::unique_ptr<ASTExpression> first, std::unique_ptr<ASTExpression> second)
            : first(std::move(first)), second(std::move(second)) {}
        void print(std::ostream& stream) const override { stream << *first << " & " << *second; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTConjunction>(first->copy(), second->copy()); }

    };

    struct ASTDisjunction final : ASTExpression {
        std::unique_ptr<ASTExpression> first;
        std::unique_ptr<ASTExpression> second;
        ASTDisjunction(std::unique_ptr<ASTExpression> first, std::unique_ptr<ASTExpression> second)
            : first(std::move(first)), second(std::move(second)) {}
        void print(std::ostream& stream) const override { stream << *first << " | " << *second; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTDisjunction>(first->copy(), second->copy()); }

    };

    struct ASTGetIndex final : ASTExpression {
        std::unique_ptr<ASTExpression> value;
        std::string index;
        ASTGetIndex(std::unique_ptr<ASTExpression> value, std::string index)
            : value(std::move(value)), index(std::move(index)) {}
        void print(std::ostream& stream) const override { stream << *value << "." << index; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTGetIndex>(value->copy(), index); }

    };

    struct ASTUpdateWith final : ASTExpression {
        std::unique_ptr<ASTExpression> original_value;
        std::unique_ptr<ASTMap> update_with;
        ASTUpdateWith(std::unique_ptr<ASTExpression> original_value, std::unique_ptr<ASTMap> update_with)
            : original_value(std::move(original_value)), update_with(std::move(update_with)) {}
        void print(std::ostream& stream) const override { stream << *original_value << " with " << *update_with; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTAddition>(original_value->copy(), update_with->copy()); }

    };

    struct ASTCurry final : ASTExpression {
        std::unique_ptr<ASTExpression> callable;
        std::unique_ptr<ASTExpression> value;
        ASTCurry(std::unique_ptr<ASTExpression> callable, std::unique_ptr<ASTExpression> value)
            : callable(std::move(callable)), value(std::move(value)) {}
        void print(std::ostream& stream) const override { stream << *callable << " " << *value; }
        Symbol& create_symbols(ProgramBuilder &builder, Context &parent) const override;
        std::unique_ptr<ASTExpression> copy() const override
        { return std::make_unique<ASTAddition>(callable->copy(), value->copy()); }


    };
}

#endif //AST_H
