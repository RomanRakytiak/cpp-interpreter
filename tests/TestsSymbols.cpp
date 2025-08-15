#include <gtest/gtest.h>
#include "program_builder.h"

using namespace traf;

TEST(SymbolTest, LiteralOverflowSubtraction) {
    ProgramBuilder builder;
    auto five = Literal::integer(5);
    auto six = Literal::integer(6);
    auto& result = six.overflow_sub(five, builder);
    auto one = dynamic_cast<Literal*>(&result);
    ASSERT_NE(nullptr, one);
    ASSERT_EQ((*one)->try_to_index().value(), 1);
}

TEST(SymbolTest, CompilationWithBinaryOperation) {
    ProgramBuilder builder;
    auto five = Literal::integer(5);
    auto six = Literal::integer(6);
    auto seven = Literal::integer(7);
    auto eight = Literal::integer(8);
    auto first = BinaryOperationResult(
        BinaryOperationResult::OVERFLOW_MULTIPLICATION,
        five,
        six
    );
    auto second = seven;
    auto result = ConditionalResult(
        eight,
        first,
        second
    );
    result.define(builder);
    ASSERT_EQ(builder.build().run().try_to_index(), 5 * 6);
}

TEST(SymbolTest, AssignWithInlineFunctionAndAddition) {
    ProgramBuilder builder;
    auto five = Literal::integer(5);
    auto six = Literal::integer(6);
    auto function = InlineFunction(
        Context(),
        std::make_unique<ASTInteger>("1"),
        {"x"}
    );
    auto &first = function.curry(five, builder);
    auto &second = first.overflow_add(six, builder);
    second.define(builder);
    ASSERT_EQ(builder.build().run().try_to_index(), 1 + 6);
}

TEST(SymbolTest, AstAssignWithConditionalFunctionCall) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "let function : x = 5 if x else () in function 0";
    auto ast = builder.compile_expression(code);
    auto context = Context();
    auto &result = ast->create_symbols(builder, context);
    result.define(builder);
    ASSERT_EQ(builder.build().run(), Variant());
}

TEST(SymbolTest, AstFieldOfMap) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "{ 1 = 1 , 2 = 2 } # 1";
    auto ast = builder.compile_expression(code);
    auto context = Context();
    auto &result = ast->create_symbols(builder, context);
    result.define(builder);
    ASSERT_EQ(builder.build().run(), Variant::integer(1));
    ASSERT_EQ(Variant::integer(1).try_to_index(), 1);
}