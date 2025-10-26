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
    std::string input = "123";  // Note: parser expects full input consumed
    
    std::shared_ptr<number_literal> ast;
    bool result = stlx::parser<number_literal>::parse(input.cbegin(), input.cend(), ast);
    
    // This will fail because number_literal is a one_or_more which doesn't match entire input
    // The parser requires consuming all input, so let's test with a proper rule wrapper
    EXPECT_TRUE(result) << "Failed to parse number '123'";
    EXPECT_NE(ast, nullptr);
}

TEST_F(ABasicGrammarTest, ParsePrimaryExpression_Number) {
    std::string input = "42";
    
    std::shared_ptr<primary_expression> ast;
    bool result = stlx::parser<primary_expression>::parse(input.cbegin(), input.cend(), ast);
    
    EXPECT_TRUE(result) << "Failed to parse number '42'";
    EXPECT_NE(ast, nullptr) << "AST should not be null";
}

TEST_F(ABasicGrammarTest, ParseUnaryExpression) {
    std::string input = "-42";
    
    std::shared_ptr<unary_expression> ast;
    bool result = stlx::parser<unary_expression>::parse(input.cbegin(), input.cend(), ast);
    
    EXPECT_TRUE(result);
    EXPECT_NE(ast, nullptr);
}

TEST_F(ABasicGrammarTest, ParseAssignmentStatement) {
    std::string input = " LET x = 42 ";
    
    std::shared_ptr<assignment_statement> ast;
    bool result = stlx::parser<assignment_statement>::parse(input.cbegin(), input.cend(), ast);
    
    EXPECT_TRUE(result);
    EXPECT_NE(ast, nullptr);
}

TEST_F(ABasicGrammarTest, ParsePrintStatement) {
    std::string input = " PRINT 123 ";
    
    std::shared_ptr<print_statement> ast;
    bool result = stlx::parser<print_statement>::parse(input.cbegin(), input.cend(), ast);
    
    EXPECT_TRUE(result);
    EXPECT_NE(ast, nullptr);
}

TEST_F(ABasicGrammarTest, ParseInputStatement) {
    std::string input = " INPUT name ";
    
    std::shared_ptr<input_statement> ast;
    bool result = stlx::parser<input_statement>::parse(input.cbegin(), input.cend(), ast);
    
    EXPECT_TRUE(result);
    EXPECT_NE(ast, nullptr);
}

TEST_F(ABasicGrammarTest, ParseIfStatement) {
    std::string input = " IF x > 5 THEN PRINT \"high\" ELSE PRINT \"low\" ENDIF ";
    
    std::shared_ptr<if_statement> ast;
    bool result = stlx::parser<if_statement>::parse(input.cbegin(), input.cend(), ast);
    
    EXPECT_TRUE(result);
    EXPECT_NE(ast, nullptr);
}

TEST_F(ABasicGrammarTest, ParseForLoop) {
    std::string input = " FOR i = 1 TO 10 STEP 1 LET x = i * 2 NEXT i ";
    
    std::shared_ptr<for_statement> ast;
    bool result = stlx::parser<for_statement>::parse(input.cbegin(), input.cend(), ast);
    
    EXPECT_TRUE(result);
    EXPECT_NE(ast, nullptr);
}

TEST_F(ABasicGrammarTest, ParseWhileLoop) {
    std::string input = " WHILE x < 10 LET x = x + 1 WEND ";
    
    std::shared_ptr<while_statement> ast;
    bool result = stlx::parser<while_statement>::parse(input.cbegin(), input.cend(), ast);
    
    EXPECT_TRUE(result);
    EXPECT_NE(ast, nullptr);
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
    
    std::shared_ptr<basic_program> ast;
    bool result = stlx::parser<basic_program>::parse(program.cbegin(), program.cend(), ast);
    
    EXPECT_TRUE(result) << "Failed to parse complete program";
    EXPECT_NE(ast, nullptr);
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
    
    std::shared_ptr<basic_program> ast;
    bool result = stlx::parser<basic_program>::parse(program.cbegin(), program.cend(), ast);
    
    EXPECT_TRUE(result) << "Failed to parse complex program";
    EXPECT_NE(ast, nullptr);
}
