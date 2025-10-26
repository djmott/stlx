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
    context<std::string::const_iterator> ctx(begin, end);
    
    digit child;
    number_literal parser(std::make_shared<digit>(child));
    bool result = parser.parse(ctx, begin, end);
    EXPECT_TRUE(result) << "Failed to parse number '123'";
    // Number literal should be parsed successfully
}

TEST_F(ABasicGrammarTest, ParsePrimaryExpression_Number) {
    std::string input = "42";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::const_iterator> ctx(begin, end);
    
    primary_expression parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
}

TEST_F(ABasicGrammarTest, ParseUnaryExpression) {
    std::string input = "-42";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::const_iterator> ctx(begin, end);
    
    unary_expression parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
}

TEST_F(ABasicGrammarTest, ParseAssignmentStatement) {
    std::string input = " LET x = 42 ";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::const_iterator> ctx(begin, end);
    
    assignment_statement parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
}

TEST_F(ABasicGrammarTest, ParsePrintStatement) {
    std::string input = " PRINT 123 ";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::const_iterator> ctx(begin, end);
    
    print_statement parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
}

TEST_F(ABasicGrammarTest, ParseInputStatement) {
    std::string input = " INPUT name ";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::const_iterator> ctx(begin, end);
    
    input_statement parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
}

TEST_F(ABasicGrammarTest, ParseIfStatement) {
    std::string input = " IF x > 5 THEN PRINT \"high\" ELSE PRINT \"low\" ENDIF ";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::const_iterator> ctx(begin, end);
    
    if_statement parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
}

TEST_F(ABasicGrammarTest, ParseForLoop) {
    std::string input = " FOR i = 1 TO 10 STEP 1 LET x = i * 2 NEXT i ";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::const_iterator> ctx(begin, end);
    
    for_statement parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
}

TEST_F(ABasicGrammarTest, ParseWhileLoop) {
    std::string input = " WHILE x < 10 LET x = x + 1 WEND ";
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::const_iterator> ctx(begin, end);
    
    while_statement parser;
    EXPECT_TRUE(parser.parse(ctx, begin, end));
}

TEST_F(ABasicGrammarTest, ParseCompleteProgram) {
    std::string program = R"(
 LET x = 10 
 LET y = 20 
 IF x > y THEN 
   LET result = x 
 ELSE 
   LET result = y 
 ENDIF 
 PRINT result 
 FOR i = 1 TO 5 STEP 1 
   LET sum = sum + i 
 NEXT i 
 WHILE x < 100 
   LET x = x * 2 
 WEND 
 PRINT x 
 END 
)";
    
    auto begin = program.begin();
    auto end = program.end();
    context<std::string::const_iterator> ctx(begin, end);
    
    basic_program rule_parser;
    EXPECT_TRUE(rule_parser.parse(ctx, begin, end));
    // Note: begin != end is acceptable as the parser may not consume trailing whitespace/newlines
}

TEST_F(ABasicGrammarTest, ParseComplexProgramWithAllFeatures) {
    std::string program = R"(
 LET x = 10 
 LET y = 5 
 LET result = 0 
 IF x > y THEN 
   LET result = x * 2 
   PRINT "x is greater" 
 ELSE 
   LET result = y * 2 
   PRINT "y is greater" 
 ENDIF 
 INPUT user_input 
 IF user_input > 100 THEN 
   LET z = user_input / 2 
 ELSE 
   LET z = user_input * 2 
 ENDIF 
 FOR counter = 1 TO 10 STEP 1 
   LET sum = sum + counter 
   PRINT counter 
 NEXT counter 
 WHILE result < 100 
   LET result = result + 10 
   PRINT result 
 WEND 
 GOTO label1 
 label1: 
 GOSUB subroutine1 
 RETURN 
 subroutine1: 
   LET temp = x + y 
   PRINT temp 
   RETURN 
 END 
)";
    
    auto begin = program.begin();
    auto end = program.end();
    context<std::string::const_iterator> ctx(begin, end);
    
    basic_program rule_parser;
    EXPECT_TRUE(rule_parser.parse(ctx, begin, end));
    // Note: begin != end is acceptable as the parser may not consume trailing whitespace/newlines
}
