// Copyright (c) 2025 David Mott
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

#include <gtest/gtest.h>
#include <stlx/parser.hpp>
#include <stlx/grammars/abasic.hpp>

using namespace stlx::parse;
using namespace stlx::grammars::basic;

class ABasicGrammarTest : public ::testing::Test {
public:
    void SetUp() override {
        // Initialize parser context
    }

    void TearDown() override {
        // Cleanup if needed
    }
};

TEST_F(ABasicGrammarTest, ParseNumber) {
    std::string input = "123";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::iterator> ctx(begin, end);
    
    number_literal parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
    EXPECT_EQ(begin, end);
}

TEST_F(ABasicGrammarTest, ParsePrimaryExpression_Number) {
    std::string input = "42";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::iterator> ctx(begin, end);
    
    primary_expression parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
}

TEST_F(ABasicGrammarTest, ParseUnaryExpression) {
    std::string input = "-42";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::iterator> ctx(begin, end);
    
    unary_expression parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
}
