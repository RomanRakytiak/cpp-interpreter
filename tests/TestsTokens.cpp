#include <gtest/gtest.h>

#include <iostream>
#include "parsing.h"

using namespace traf;

TEST(LexerTest, EqualityWithStringView) {
    location loc{};
    token t{token::NAME, "hello", loc, loc};
    ASSERT_EQ(t, "hello");
    ASSERT_NE(t, "world");
}

TEST(LexerTest, BasicTokenParsing) {
    lexer l("name 42 +", "test");

    token t1 = l.next_token();
    ASSERT_EQ(t1.type, token::NAME);
    ASSERT_EQ(t1, "name");

    token t2 = l.next_token();
    ASSERT_EQ(t2.type, token::NUMBER);
    ASSERT_EQ(t2, "42");

    token t3 = l.next_token();
    ASSERT_EQ(t3.type, token::OPERATOR);
    ASSERT_EQ(t3, "+");
}

TEST(LexerTest, OperatorCollisions) {
    lexer l("!+-/'-*+?<?", "test");

    token t1 = l.next_token();
    ASSERT_EQ(t1.type, token::OPERATOR);
    ASSERT_EQ(t1, "!+-/");

    token t2 = l.next_token();
    ASSERT_EQ(t2.type, token::OPERATOR);
    ASSERT_EQ(t2, "'-*+?");

    token t3 = l.next_token();
    ASSERT_EQ(t3.type, token::OPERATOR);
    ASSERT_EQ(t3, "<?");
}

TEST(LexerTest, Numbers) {
    lexer l("156 1.555.8'8. .1_2", "test");

    token t1 = l.next_token();
    ASSERT_EQ(t1.type, token::NUMBER);
    ASSERT_EQ(t1, "156");

    token t2 = l.next_token();
    ASSERT_EQ(t2.type, token::NUMBER);
    ASSERT_EQ(t2, "1.555");

    token t3 = l.next_token();
    ASSERT_EQ(t3.type, token::OPERATOR);
    ASSERT_EQ(t3, ".");

    token t4 = l.next_token();
    ASSERT_EQ(t4.type, token::NUMBER);
    ASSERT_EQ(t4, "8'8");

    l.skip_token();

    token t5 = l.next_token();
    ASSERT_EQ(t5.type, token::OPERATOR);
    ASSERT_EQ(t5, ".");

    token t6 = l.next_token();
    ASSERT_EQ(t6.type, token::NUMBER);
    ASSERT_EQ(t6, "1_2");
}
