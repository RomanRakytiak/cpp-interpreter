#include <gtest/gtest.h>
#include "BytecodeBuilder.h"

using namespace project;

TEST(BytecodeBuilderTest, NextConstantRef_ReturnsCorrectReference) {
    const BytecodeBuilder builder;
    const auto constant = builder.next_constant_address();

    ASSERT_EQ(constant, ConstantAddress(0));
}

TEST(BytecodeBuilderTest, NextInstructionRef_ReturnsCorrectReference) {
    const BytecodeBuilder builder;
    const auto instruction = builder.next_instruction_address();

    ASSERT_EQ(instruction, InstructionAddress(0));
}

TEST(BytecodeBuilderTest, PushIntegralValue) {
    BytecodeBuilder builder;
    constexpr Program::word value = 42;
    const StackAddress ref = builder.push(value);

    ASSERT_EQ(ref, StackAddress(0));
    ASSERT_EQ(builder.instruction_at(0).type, Program::PUSH_IMMEDIATE);
    ASSERT_EQ(builder.instruction_at(0).argument, value);
}

TEST(BytecodeBuilderTest, PushDoubleValue) {
    BytecodeBuilder builder;
    constexpr float value = 3.14;
    const StackAddress ref = builder.push(value);

    ASSERT_EQ(ref, StackAddress(0));
    ASSERT_EQ(builder.next_constant_address(), ConstantAddress(1));
    ASSERT_EQ(builder.instruction_at(0).type, Program::PUSH_CONST);
    ASSERT_EQ(builder.constant_at(0), Variant::floating_point(value));
}

TEST(BytecodeBuilderTest, PushConstantRefValue) {
    BytecodeBuilder builder;
    const auto constant = builder.new_constant(Variant::integer(5));

    builder.push(constant);

    ASSERT_EQ(builder.next_constant_address(), ConstantAddress(1));
    ASSERT_EQ(builder.next_instruction_address(), InstructionAddress(1));
}

TEST(BytecodeBuilderTest, StackTopReturnsLastPushedValue) {
    BytecodeBuilder builder;

    builder.push(42);
    builder.push(7);

    ASSERT_EQ(builder.stack_top(), StackAddress(1));
}

TEST(BytecodeBuilderTest, PopRemovesTopElement) {
    BytecodeBuilder builder;

    builder.push(42);
    builder.push(7);

    builder.pop();
    ASSERT_EQ(builder.stack_top(), StackAddress(0));
    ASSERT_EQ(builder.next_instruction_address(), InstructionAddress(3));
}

TEST(BytecodeBuilderTest, SwapTopWithIndexSwapsCorrectly) {
    BytecodeBuilder builder;

    builder.push(42);
    builder.push(7);

    builder.swap_top_with(1);

    ASSERT_EQ(builder.stack_top(), StackAddress(1));
    ASSERT_EQ(builder.instruction_at(0).type, Program::PUSH_IMMEDIATE);
    ASSERT_EQ(builder.instruction_at(1).type, Program::PUSH_IMMEDIATE);
    ASSERT_EQ(builder.instruction_at(2).type, Program::SWAP);
    ASSERT_EQ(builder.instruction_at(2).argument, 1);
}

TEST(BytecodeBuilderTest, AddAddsCorrectly) {
    BytecodeBuilder builder;

    builder.push(3);
    builder.push(4);

    ASSERT_EQ(builder.stack_top(), StackAddress(1));

    builder.command(Program::OVERFLOW_ADD);
    const StackAddress result_ref = builder.stack_top();

    ASSERT_EQ(result_ref, StackAddress(0));
    ASSERT_EQ(builder.stack_top(), StackAddress(0));
    ASSERT_EQ(builder.instruction_at(0).type, Program::PUSH_IMMEDIATE);
    ASSERT_EQ(builder.instruction_at(1).type, Program::PUSH_IMMEDIATE);
    ASSERT_EQ(builder.instruction_at(2).type, Program::OVERFLOW_ADD);
}

TEST(BytecodeBuilderTest, BuildReturnsCorrectProgram) {
    BytecodeBuilder builder;

    builder.push(3);
    builder.push(5);
    builder.push(5.4);

    const Program program = builder.build();

    ASSERT_EQ(program.instructions.size(), 3);
    ASSERT_EQ(program.constants.size(), 1);
}

TEST(BytecodeBuilderTest, EvaluateOperatorReturnsCorrectInstruction) {
    const auto instruction = BytecodeBuilder::evaluate_operator('+');

    ASSERT_TRUE(instruction.has_value());
    ASSERT_EQ(instruction.value(), Program::OVERFLOW_ADD);
}