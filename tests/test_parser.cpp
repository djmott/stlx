/**
 * @file test_parser.cpp
 * @brief Unit tests for STLX parser combinator library
 *
 * This file contains comprehensive unit tests for the STLX parser combinator
 * library including character parsing, combinators, quantifiers, and error
 * handling.
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#include "stlx/parser.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace stlx::parse;

class ParserTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Common setup if needed
  }

  void TearDown() override {
    // Common cleanup if needed
  }
};

// Test basic character parsing using parser::parse
TEST_F(ParserTest, CharacterParsing) {
  struct char_a_rule : rule<std::string::iterator, char_a_rule,
                            and_<std::string::iterator,
                                character<std::string::iterator, 'a'>,
                                eof<std::string::iterator>>> {};

  std::string input1 = "a";
  std::string input2 = "b";

  std::shared_ptr<char_a_rule> ast1, ast2;
  
  EXPECT_TRUE(stlx::parser<char_a_rule>::parse(input1.begin(), input1.end(), ast1));
  EXPECT_FALSE(stlx::parser<char_a_rule>::parse(input2.begin(), input2.end(), ast2));
  
  EXPECT_NE(ast1, nullptr) << "AST should not be null for valid input";
  
  if (ast1) {
    const std::type_info& type_info = ast1->type();
    EXPECT_EQ(type_info, typeid(char_a_rule)) << "type() should return char_a_rule type";
    EXPECT_TRUE(ast1->isa(typeid(char_a_rule))) << "isa() should identify char_a_rule";
    EXPECT_FALSE(ast1->name().empty()) << "name() should return non-empty string";
  }
}

// Test string parsing
TEST_F(ParserTest, StringParsing) {
  struct char_h_rule : rule<std::string::iterator, char_h_rule,
                            and_<std::string::iterator,
                                character<std::string::iterator, 'h'>,
                                eof<std::string::iterator>>> {};

  std::string input1 = "h";
  std::string input2 = "w";

  std::shared_ptr<char_h_rule> ast1, ast2;
  
  EXPECT_TRUE(stlx::parser<char_h_rule>::parse(input1.begin(), input1.end(), ast1));
  EXPECT_FALSE(stlx::parser<char_h_rule>::parse(input2.begin(), input2.end(), ast2));
  
  if (ast1) {
    EXPECT_EQ(ast1->type(), typeid(char_h_rule));
    EXPECT_TRUE(ast1->isa(typeid(char_h_rule)));
    EXPECT_FALSE(ast1->name().empty());
  }
}

// Test OR combinator with parser::parse
TEST_F(ParserTest, OrCombinator) {
  using char_a = character<std::string::iterator, 'a'>;
  using char_b = character<std::string::iterator, 'b'>;
  
  struct or_rule : rule<std::string::iterator, or_rule,
                        or_<std::string::iterator,
                            character<std::string::iterator, 'a'>,
                            character<std::string::iterator, 'b'>>> {};

  std::string input1 = "a";
  std::string input2 = "b";
  std::string input3 = "c";

  std::shared_ptr<or_rule> ast1, ast2, ast3;
  
  EXPECT_TRUE(stlx::parser<or_rule>::parse(input1.begin(), input1.end(), ast1, false));
  EXPECT_TRUE(stlx::parser<or_rule>::parse(input2.begin(), input2.end(), ast2, false));
  EXPECT_FALSE(stlx::parser<or_rule>::parse(input3.begin(), input3.end(), ast3, false));
  
  if (ast1) {
    EXPECT_EQ(ast1->type(), typeid(or_rule));
    EXPECT_TRUE(ast1->isa(typeid(or_rule)));
    EXPECT_FALSE(ast1->name().empty());
  }
}

// Test AND combinator with parser::parse
TEST_F(ParserTest, AndCombinator) {
  struct and_rule : rule<std::string::iterator, and_rule,
                         and_<std::string::iterator,
                             character<std::string::iterator, 'a'>,
                             character<std::string::iterator, 'b'>>> {};

  std::string input1 = "ab";
  std::string input2 = "ac";

  std::shared_ptr<and_rule> ast1, ast2;
  
  EXPECT_TRUE(stlx::parser<and_rule>::parse(input1.begin(), input1.end(), ast1, false));
  EXPECT_FALSE(stlx::parser<and_rule>::parse(input2.begin(), input2.end(), ast2, false));
  
  if (ast1) {
    EXPECT_EQ(ast1->type(), typeid(and_rule));
    EXPECT_TRUE(ast1->isa(typeid(and_rule)));
  }
}

// Test NOT combinator with parser::parse
TEST_F(ParserTest, NotCombinator) {
  struct not_rule : rule<std::string::iterator, not_rule,
                          not_<std::string::iterator,
                              character<std::string::iterator, 'a'>>> {};

  std::string input1 = "b";
  std::string input2 = "a";

  std::shared_ptr<not_rule> ast1, ast2;
  
  EXPECT_TRUE(stlx::parser<not_rule>::parse(input1.begin(), input1.end(), ast1, false));
  EXPECT_FALSE(stlx::parser<not_rule>::parse(input2.begin(), input2.end(), ast2, false));
  
  if (ast1) {
    EXPECT_EQ(ast1->type(), typeid(not_rule));
  }
}

// Test ZERO_OR_MORE combinator with parser::parse
TEST_F(ParserTest, ZeroOrMoreCombinator) {
  struct zero_or_more_rule : rule<std::string::iterator, zero_or_more_rule,
                                  zero_or_more_<std::string::iterator,
                                               character<std::string::iterator, 'a'>>> {};

  std::string input1 = "aaaa";
  std::string input2 = "";
  std::string input3 = "aaab";

  std::shared_ptr<zero_or_more_rule> ast1, ast2, ast3;
  
  EXPECT_TRUE(stlx::parser<zero_or_more_rule>::parse(input1.begin(), input1.end(), ast1, false));
  EXPECT_TRUE(stlx::parser<zero_or_more_rule>::parse(input2.begin(), input2.end(), ast2, false));
  EXPECT_TRUE(stlx::parser<zero_or_more_rule>::parse(input3.begin(), input3.end(), ast3, false));
  
  if (ast1 && ast2 && ast3) {
    EXPECT_EQ(ast1->size(), 4); // Should have 4 matches
    EXPECT_EQ(ast2->size(), 0); // Should have 0 matches
    EXPECT_EQ(ast3->size(), 3); // Should have 3 matches
  }
}

// Test ZERO_OR_MORE combinator with CRTP struct and type verification
TEST_F(ParserTest, ZeroOrMoreCombinatorWithCRTP) {
  struct zero_or_more_rule : rule<std::string::iterator, zero_or_more_rule,
                                  zero_or_more_<std::string::iterator,
                                               character<std::string::iterator, 'a'>>> {};

  std::string input1 = "aaaa";
  std::string input2 = "";
  std::string input3 = "aaab";

  std::shared_ptr<zero_or_more_rule> ast1, ast2, ast3;
  
  EXPECT_TRUE(stlx::parser<zero_or_more_rule>::parse(input1.begin(), input1.end(), ast1, false));
  EXPECT_TRUE(stlx::parser<zero_or_more_rule>::parse(input2.begin(), input2.end(), ast2, false));
  EXPECT_TRUE(stlx::parser<zero_or_more_rule>::parse(input3.begin(), input3.end(), ast3, false));
  
  if (ast1 && ast2 && ast3) {
    EXPECT_EQ(ast1->size(), 4);
    EXPECT_EQ(ast2->size(), 0);
    EXPECT_EQ(ast3->size(), 3);
    
    EXPECT_EQ(ast1->type(), typeid(zero_or_more_rule));
    EXPECT_TRUE(ast1->isa(typeid(zero_or_more_rule)));
    EXPECT_FALSE(ast1->name().empty());
  }
}

// Test parser name() method returns readable names
TEST_F(ParserTest, ParserNameMethod) {
  struct test_or : rule<std::string::iterator, test_or,
                        or_<std::string::iterator,
                            character<std::string::iterator, 'a'>,
                            character<std::string::iterator, 'b'>>> {};
  
  struct test_and : rule<std::string::iterator, test_and,
                         and_<std::string::iterator,
                             character<std::string::iterator, 'a'>,
                             character<std::string::iterator, 'b'>>> {};
  
  std::string input = "a";
  
  std::shared_ptr<test_or> ast_or;
  std::shared_ptr<test_and> ast_and;
  
  stlx::parser<test_or>::parse(input.begin(), input.end(), ast_or, false);
  
  if (ast_or) {
    std::string or_name = ast_or->name();
    EXPECT_FALSE(or_name.empty());
    EXPECT_LT(or_name.length(), 200);
    EXPECT_TRUE(or_name.find("test_or") != std::string::npos);
    
    EXPECT_EQ(ast_or->type(), typeid(test_or));
    EXPECT_TRUE(ast_or->isa(typeid(test_or)));
  }
  
  input = "ab";
  stlx::parser<test_and>::parse(input.begin(), input.end(), ast_and, false);
  
  if (ast_and) {
    std::string and_name = ast_and->name();
    EXPECT_FALSE(and_name.empty());
    EXPECT_LT(and_name.length(), 200);
    EXPECT_TRUE(and_name.find("test_and") != std::string::npos);
    
    EXPECT_EQ(ast_and->type(), typeid(test_and));
    EXPECT_TRUE(ast_and->isa(typeid(test_and)));
  }
}

// Test parser type() method returns correct type
TEST_F(ParserTest, ParserTypeMethod) {
  struct test_rule : rule<std::string::iterator, test_rule,
                          character<std::string::iterator, 'a'>> {};
  
  std::string input = "a";
  std::shared_ptr<test_rule> ast;
  
  EXPECT_TRUE(stlx::parser<test_rule>::parse(input.begin(), input.end(), ast, false));
  
  if (ast) {
    EXPECT_EQ(ast->type(), typeid(test_rule));
    EXPECT_TRUE(ast->isa(typeid(test_rule)));
    EXPECT_TRUE(ast->isa(typeid(ast)));
  }
}

// Test ONE_OR_MORE combinator with parser::parse
TEST_F(ParserTest, OneOrMoreCombinator) {
  struct one_or_more_rule : rule<std::string::iterator, one_or_more_rule,
                                  one_or_more_<std::string::iterator,
                                              character<std::string::iterator, 'a'>>> {};

  std::string input1 = "aaaa";
  std::string input2 = "";

  std::shared_ptr<one_or_more_rule> ast1, ast2;
  
  EXPECT_TRUE(stlx::parser<one_or_more_rule>::parse(input1.begin(), input1.end(), ast1, false));
  EXPECT_FALSE(stlx::parser<one_or_more_rule>::parse(input2.begin(), input2.end(), ast2, false));
  
  if (ast1) {
    EXPECT_EQ(ast1->size(), 4);
  }
}

// Test ZERO_OR_ONE combinator with parser::parse
TEST_F(ParserTest, ZeroOrOneCombinator) {
  struct zero_or_one_rule : rule<std::string::iterator, zero_or_one_rule,
                                  zero_or_one_<std::string::iterator,
                                              character<std::string::iterator, 'a'>>> {};

  std::string input1 = "a";
  std::string input2 = "";
  std::string input3 = "aa";

  std::shared_ptr<zero_or_one_rule> ast1, ast2, ast3;
  
  EXPECT_TRUE(stlx::parser<zero_or_one_rule>::parse(input1.begin(), input1.end(), ast1, false));
  EXPECT_TRUE(stlx::parser<zero_or_one_rule>::parse(input2.begin(), input2.end(), ast2, false));
  EXPECT_TRUE(stlx::parser<zero_or_one_rule>::parse(input3.begin(), input3.end(), ast3, false));
  
  if (ast1 && ast2 && ast3) {
    EXPECT_EQ(ast1->size(), 1);
    EXPECT_EQ(ast2->size(), 0);
    EXPECT_EQ(ast3->size(), 1);
  }
}

// Test whitespace handling with parser::parse
TEST_F(ParserTest, WhitespaceHandling) {
  struct char_a_rule : rule<std::string::iterator, char_a_rule,
                            character<std::string::iterator, 'a'>> {};

  std::string input1 = "  a";
  std::string input2 = "  b";

  std::shared_ptr<char_a_rule> ast1, ast2;
  
  EXPECT_TRUE(stlx::parser<char_a_rule>::parse(input1.begin(), input1.end(), ast1, false));
  EXPECT_FALSE(stlx::parser<char_a_rule>::parse(input2.begin(), input2.end(), ast2, false));
}

// Test error reporting with parser::parse
TEST_F(ParserTest, ErrorReporting) {
  struct char_a_rule : rule<std::string::iterator, char_a_rule,
                         character<std::string::iterator, 'a'>> {};

  std::string input = "b";
  
  std::shared_ptr<char_a_rule> ast;
  typename parse_error<std::string::iterator>::vector errors;
  
  EXPECT_FALSE(stlx::parser<char_a_rule>::parse(input.begin(), input.end(), ast, errors, false));
  EXPECT_FALSE(errors.empty());
}

// Test main parser interface
TEST_F(ParserTest, MainParserInterface) {
  struct char_a_rule : rule<std::string::iterator, char_a_rule,
                            character<std::string::iterator, 'a'>> {};

  std::string input = "a";
  std::string input2 = "b";

  std::shared_ptr<char_a_rule> ast, ast2;
  
  typename parse_error<std::string::iterator>::vector errors, errors2;

  EXPECT_TRUE(stlx::parser<char_a_rule>::parse(input.begin(), input.end(), ast, errors, false));
  EXPECT_FALSE(stlx::parser<char_a_rule>::parse(input2.begin(), input2.end(), ast2, errors2, false));
}

// Test complex expression parsing with parser::parse
TEST_F(ParserTest, ComplexExpressionParsing) {
  struct digit_sequence : rule<std::string::iterator, digit_sequence,
                              one_or_more_<std::string::iterator,
                                          characters<std::string::iterator, '0', '9'>>> {};

  std::string input = "123";
  
  std::shared_ptr<digit_sequence> ast;
  
  EXPECT_TRUE(stlx::parser<digit_sequence>::parse(input.begin(), input.end(), ast, false));
  
  if (ast) {
    EXPECT_EQ(ast->size(), 3);
  }
}

// Test edge cases with parser::parse
TEST_F(ParserTest, EdgeCases) {
  struct char_a_rule : rule<std::string::iterator, char_a_rule,
                            character<std::string::iterator, 'a'>> {};

  std::string empty = "";
  std::string single = "a";

  std::shared_ptr<char_a_rule> ast_empty, ast_single;
  
  EXPECT_FALSE(stlx::parser<char_a_rule>::parse(empty.begin(), empty.end(), ast_empty, false));
  EXPECT_TRUE(stlx::parser<char_a_rule>::parse(single.begin(), single.end(), ast_single, false));
}

// Performance test with parser::parse
TEST_F(ParserTest, PerformanceTest) {
  struct digit_sequence : rule<std::string::iterator, digit_sequence,
                              one_or_more_<std::string::iterator,
                                          characters<std::string::iterator, '0', '9'>>> {};

  std::string input(10000, '1');
  
  std::shared_ptr<digit_sequence> ast;
  
  EXPECT_TRUE(stlx::parser<digit_sequence>::parse(input.begin(), input.end(), ast, false));
  
  if (ast) {
    EXPECT_EQ(ast->size(), 10000);
  }
}

// Test EOF rule with parser::parse
TEST_F(ParserTest, EOFRule) {
  struct eof_wrapper : rule<std::string::iterator, eof_wrapper,
                            and_<std::string::iterator,
                                character<std::string::iterator, 'h'>,
                                eof<std::string::iterator>>> {};

  std::string input1 = "h";
  std::string input2 = "hello";
  
  std::shared_ptr<eof_wrapper> ast1, ast2;
  
  EXPECT_TRUE(stlx::parser<eof_wrapper>::parse(input1.begin(), input1.end(), ast1, false));
  EXPECT_FALSE(stlx::parser<eof_wrapper>::parse(input2.begin(), input2.end(), ast2, false));
}
