#include <gtest/gtest.h>
#include "program_builder.h"

using namespace traf;

TEST(ASTTest, ASTAdditionAndEvaluate) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "15 + 1.5 + x";
    auto ast = builder.compile_expression(code);

    const auto first_add = dynamic_cast<ASTAddition*>(ast.get());
    ASSERT_NE(first_add, nullptr);
    const auto second_add = dynamic_cast<ASTAddition*>(first_add->first.get());
    ASSERT_NE(second_add, nullptr);

    const auto first = dynamic_cast<ASTInteger*>(second_add->first.get());
    ASSERT_NE(first, nullptr);
    ASSERT_EQ(first->value, "15");

    const auto second = dynamic_cast<ASTFloat*>(second_add->second.get());
    ASSERT_NE(second, nullptr);
    ASSERT_EQ(second->value, "1.5");

    const auto third = dynamic_cast<ASTEvaluate*>(first_add->second.get());
    ASSERT_NE(third, nullptr);
    ASSERT_EQ(third->name, "x");
}

TEST(ASTTest, ASTSingleInteger) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "42";
    auto ast = builder.compile_expression(code);

    const auto value = dynamic_cast<ASTInteger*>(ast.get());
    ASSERT_NE(value, nullptr);
    ASSERT_EQ(value->value, "42");
}



TEST(ASTTest, TemplateFor) {
    template_for<int, 1, 5>([]<int I>(){ std::cout << I << std::endl; });
}

TEST(ASTTest, ASTLetExpressionWithCondition) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "let x = 4 in x if 5 else 10";
    auto ast = builder.compile_expression(code);

    const auto let_in = dynamic_cast<ASTLetExpression*>(ast.get());
    ASSERT_NE(let_in, nullptr);

    const auto if_exp = dynamic_cast<ASTBranch*>(let_in->then_do.get());
    ASSERT_NE(if_exp, nullptr);

    const auto condition = dynamic_cast<ASTInteger*>(if_exp->condition.get());
    ASSERT_NE(condition, nullptr);
    ASSERT_EQ(condition->value, "5");

    const auto then_exp = dynamic_cast<ASTEvaluate*>(if_exp->then_do.get());
    ASSERT_NE(then_exp, nullptr);
    ASSERT_EQ(then_exp->name, "x");

    const auto else_exp = dynamic_cast<ASTInteger*>(if_exp->else_do.get());
    ASSERT_NE(else_exp, nullptr);
    ASSERT_EQ(else_exp->value, "10");
}

TEST(ASTTest, ASTMapWithMultipleEntries) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "{ abc = xyz , 012 = 987 }";
    auto ast = builder.compile_expression(code);

    const auto value = dynamic_cast<ASTMap*>(ast.get());
    ASSERT_NE(value, nullptr);
    ASSERT_EQ(value->map.size(), 2);
}

TEST(ASTTest, ASTLetFunctionWithValue) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "let function : x = 5 in 42";
    auto ast = builder.compile_expression(code);

    const auto let_in = dynamic_cast<ASTLetExpression*>(ast.get());
    ASSERT_NE(let_in, nullptr);

    const auto function = dynamic_cast<ASTFunction*>(let_in->assign.get());
    ASSERT_NE(function, nullptr);
    ASSERT_EQ(function->parameter_names.at(0), "x");
}

TEST(ASTTest, ASTComplexExpressionWithLetAndAdditions) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "let x = 10 in 15 + x + 1.5";
    auto ast = builder.compile_expression(code);

    const auto let_in = dynamic_cast<ASTLetExpression*>(ast.get());
    ASSERT_NE(let_in, nullptr);

    const auto let_value = dynamic_cast<ASTInteger*>(let_in->assign.get());
    ASSERT_NE(let_value, nullptr);
    ASSERT_EQ(let_value->value, "10");

    const auto first_add = dynamic_cast<ASTAddition*>(let_in->then_do.get());
    ASSERT_NE(first_add, nullptr);

    const auto second_add = dynamic_cast<ASTAddition*>(first_add->first.get());
    ASSERT_NE(second_add, nullptr);

    const auto first = dynamic_cast<ASTInteger*>(second_add->first.get());
    ASSERT_NE(first, nullptr);
    ASSERT_EQ(first->value, "15");

    const auto second = dynamic_cast<ASTEvaluate*>(second_add->second.get());
    ASSERT_NE(second, nullptr);
    ASSERT_EQ(second->name, "x");

    const auto third = dynamic_cast<ASTFloat*>(first_add->second.get());
    ASSERT_NE(third, nullptr);
    ASSERT_EQ(third->value, "1.5");
}

TEST(ASTTest, ASTComplexLetWithIfElseAndMap) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "let abc = 5 in { key = abc if 1 else 0 , another_key = 42 }";
    auto ast = builder.compile_expression(code);

    const auto let_in = dynamic_cast<ASTLetExpression*>(ast.get());
    ASSERT_NE(let_in, nullptr);

    const auto let_value = dynamic_cast<ASTInteger*>(let_in->assign.get());
    ASSERT_NE(let_value, nullptr);
    ASSERT_EQ(let_value->value, "5");

    const auto map = dynamic_cast<ASTMap*>(let_in->then_do.get());
    ASSERT_NE(map, nullptr);
    ASSERT_EQ(map->map.size(), 2);

    const auto first_entry = dynamic_cast<ASTBranch*>(map->map.at("key").get());
    ASSERT_NE(first_entry, nullptr);

    const auto condition = dynamic_cast<ASTInteger*>(first_entry->condition.get());
    ASSERT_NE(condition, nullptr);
    ASSERT_EQ(condition->value, "1");

    const auto then_exp = dynamic_cast<ASTEvaluate*>(first_entry->then_do.get());
    ASSERT_NE(then_exp, nullptr);
    ASSERT_EQ(then_exp->name, "abc");

    const auto else_exp = dynamic_cast<ASTInteger*>(first_entry->else_do.get());
    ASSERT_NE(else_exp, nullptr);
    ASSERT_EQ(else_exp->value, "0");

    const auto second_entry = dynamic_cast<ASTInteger*>(map->map.at("another_key").get());
    ASSERT_NE(second_entry, nullptr);
    ASSERT_EQ(second_entry->value, "42");
}