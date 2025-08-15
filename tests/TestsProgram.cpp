#include <gtest/gtest.h>
#include "bytecode_builder.h"

using namespace traf;


TEST(ProgramTest, OverflowSubtractionTest) {
    BytecodeBuilder builder;

    builder.push(42);
    builder.push(7);
    builder.command(Program::OVERFLOW_SUB);

    const Program program = builder.build();

    ASSERT_EQ(program.instructions.at(0).type, Program::PUSH_IMMEDIATE);
    ASSERT_EQ(program.instructions.at(1).type, Program::PUSH_IMMEDIATE);
    ASSERT_EQ(program.instructions.at(2).type, Program::OVERFLOW_SUB);

    const auto result = program.execute();
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.at(0), Variant::integer(35));
}

TEST(ProgramTest, AdditionOfFloatsTest) {
    BytecodeBuilder builder;

    builder.add(0.52, 1.2);

    const Program program = builder.build();
    const auto result = program.execute();
    ASSERT_EQ(result.size(), 1);
}

TEST(ProgramTest, SwapTopWithTest) {
    BytecodeBuilder builder;

    builder.push(7);
    builder.push(42);
    builder.swap_top_with();

    const Program program = builder.build();
    const auto result = program.execute();
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result.at(0), Variant::integer(42));
    ASSERT_EQ(result.at(1), Variant::integer(7));
}

TEST(ProgramTest, DeleteElementTest) {
    BytecodeBuilder builder;

    const auto ref = builder.push(7);
    builder.try_delete(ref);

    const Program program = builder.build();
    const auto result = program.execute();
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.at(0), Variant());
}

TEST(ProgramTest, ComplexOperations) {
    BytecodeBuilder builder;

    builder.push(5);
    builder.push(3);
    builder.command(Program::OVERFLOW_MUL);
    builder.push(2);
    builder.command(Program::OVERFLOW_ADD);

    const Program program = builder.build();
    const auto result = program.execute();

    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.at(0), Variant::integer(17));
}

TEST(ProgramTest, NewConstant) {
    BytecodeBuilder builder;

    auto ref = builder.new_constant(Variant::integer(35));
    builder.push(ref);
    builder.push(ref);
    builder.command(Program::OVERFLOW_ADD);

    const Program program = builder.build();
    const auto result = program.execute();

    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.at(0), Variant::integer(70));
}