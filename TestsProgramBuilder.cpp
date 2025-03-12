#include <fstream>
#include <gtest/gtest.h>
#include "ProgramBuilder.h"

using namespace project;

TEST(ProgramBuilderTest, WriteToFile) {
    auto name = "write_test.txt";
    std::ofstream file(name);
    ASSERT_TRUE(file.is_open());
    ProgramBuilder builder;
    std::stringstream code;
    code << "write 12 ; write 12 to file";
    Context context;
    context.new_symbol("write", builder.new_literal(
        Literal::function(
            [&](auto integer) {
                file << integer.try_to_index().value();
                return Variant();
            }
    )));
    builder.compile(code, context);
    auto program = builder.build();
    ASSERT_EQ(program.run(), Variant());
    file.close();
    std::ifstream read(name);
    std::string value;
    read >> value;
    ASSERT_EQ(value, "12");
}

TEST(ProgramBuilderTest, ReadFromFile) {
    auto name = "read_test.txt";
    std::ifstream read(name);
    if (read.is_open() == false) {
        std::ofstream file(name);
        file << "let x = 5 in let y = 7 * 8 in { 1 = x } with { 2 = y }";
        file.close();
        read.open(name);
    }
    ProgramBuilder builder;
    auto ast = builder.compile_expression(read);
    Context context;
    ast->create_symbols(builder, context).define(builder);
    auto program = builder.build();
    auto result = program.run();
    std::cerr << result << std::endl;
    ASSERT_EQ(
        result.get(Variant::integer(1))->try_to_index().value(),
        5
    );
    ASSERT_EQ(
        result.get(Variant::integer(2))->try_to_index().value(),
        7 * 8
    );
}


TEST(ProgramBuilderTest, LetError) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "let abc = 5";
    try {
        auto ast = builder.compile_expression(code);
        FAIL();
    } catch (...) {
    }

}

TEST(ProgramBuilderTest, WithError) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "{ 1 = 2 } with";
    try {
        auto ast = builder.compile_expression(code);
        FAIL();
    } catch (...) {
    }
}

TEST(ProgramBuilderTest, SymbolError) {
    ProgramBuilder builder;
    std::stringstream code;
    code << "() + 5";
    try {
        builder.compile(code);
        FAIL();
    } catch (...) {
    }
}