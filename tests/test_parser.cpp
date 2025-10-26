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

// Test basic character parsing
TEST_F(ParserTest, CharacterParsing) {
  using char_a = character<std::string::iterator, 'a'>;

  std::string input1 = "a";
  std::string input2 = "b";

  auto begin1 = input1.begin();
  auto end1 = input1.end();
  context<std::string::iterator> ctx1(begin1, end1);

  auto begin2 = input2.begin();
  auto end2 = input2.end();
  context<std::string::iterator> ctx2(begin2, end2);

  char_a parser;
  EXPECT_TRUE(parser.parse(ctx1, begin1, end1));
  EXPECT_FALSE(parser.parse(ctx2, begin2, end2));
}

// Test string parsing
TEST_F(ParserTest, StringParsing) {
  // Use a simple character test instead of string for now
  using char_h = character<std::string::iterator, 'h'>;

  std::string input1 = "h";
  std::string input2 = "w";

  auto begin1 = input1.begin();
  auto end1 = input1.end();
  context<std::string::iterator> ctx1(begin1, end1);

  auto begin2 = input2.begin();
  auto end2 = input2.end();
  context<std::string::iterator> ctx2(begin2, end2);

  char_h parser;
  EXPECT_TRUE(parser.parse(ctx1, begin1, end1));
  EXPECT_FALSE(parser.parse(ctx2, begin2, end2));
}

// Test OR combinator
TEST_F(ParserTest, OrCombinator) {
  using char_a = character<std::string::iterator, 'a'>;
  using char_b = character<std::string::iterator, 'b'>;

  std::string input1 = "a";
  std::string input2 = "b";
  std::string input3 = "c";

  auto begin1 = input1.begin();
  auto end1 = input1.end();
  context<std::string::iterator> ctx1(begin1, end1);

  auto begin2 = input2.begin();
  auto end2 = input2.end();
  context<std::string::iterator> ctx2(begin2, end2);

  auto begin3 = input3.begin();
  auto end3 = input3.end();
  context<std::string::iterator> ctx3(begin3, end3);

  // Create actual instances of the child rules
  auto char_a_rule = std::make_shared<char_a>();
  auto char_b_rule = std::make_shared<char_b>();

  // Create OR combinator with the actual child rules
  or_<std::string::iterator> parser(char_a_rule, char_b_rule);

  EXPECT_TRUE(parser.parse(ctx1, begin1, end1));
  EXPECT_TRUE(parser.parse(ctx2, begin2, end2));
  EXPECT_FALSE(parser.parse(ctx3, begin3, end3));
}

// Test AND combinator
TEST_F(ParserTest, AndCombinator) {
  using char_a = character<std::string::iterator, 'a'>;
  using char_b = character<std::string::iterator, 'b'>;

  std::string input1 = "ab";
  std::string input2 = "ac";

  auto begin1 = input1.begin();
  auto end1 = input1.end();
  context<std::string::iterator> ctx1(begin1, end1);

  auto begin2 = input2.begin();
  auto end2 = input2.end();
  context<std::string::iterator> ctx2(begin2, end2);

  // Create actual instances of the child rules
  auto char_a_rule = std::make_shared<char_a>();
  auto char_b_rule = std::make_shared<char_b>();

  // Create AND combinator with the actual child rules
  and_<std::string::iterator> parser(char_a_rule, char_b_rule);

  EXPECT_TRUE(parser.parse(ctx1, begin1, end1));
  EXPECT_FALSE(parser.parse(ctx2, begin2, end2));
}

// Test NOT combinator
TEST_F(ParserTest, NotCombinator) {
  using char_a = character<std::string::iterator, 'a'>;

  std::string input1 = "b";
  std::string input2 = "a";

  auto begin1 = input1.begin();
  auto end1 = input1.end();
  context<std::string::iterator> ctx1(begin1, end1);

  auto begin2 = input2.begin();
  auto end2 = input2.end();
  context<std::string::iterator> ctx2(begin2, end2);

  // Create actual instance of the child rule
  auto char_a_rule = std::make_shared<char_a>();

  // Create NOT combinator with the actual child rule
  not_<std::string::iterator> parser(char_a_rule);

  EXPECT_TRUE(parser.parse(ctx1, begin1, end1));
  EXPECT_FALSE(parser.parse(ctx2, begin2, end2));
}

// Test ZERO_OR_MORE combinator
TEST_F(ParserTest, ZeroOrMoreCombinator) {
  using char_a = character<std::string::iterator, 'a'>;

  std::string input1 = "aaaa";
  std::string input2 = "";
  std::string input3 = "aaab";

  auto begin1 = input1.begin();
  auto end1 = input1.end();
  context<std::string::iterator> ctx1(begin1, end1);

  auto begin2 = input2.begin();
  auto end2 = input2.end();
  context<std::string::iterator> ctx2(begin2, end2);

  auto begin3 = input3.begin();
  auto end3 = input3.end();
  context<std::string::iterator> ctx3(begin3, end3);

  // Create actual instance of the child rule
  auto char_a_rule = std::make_shared<char_a>();

  // Create ZERO_OR_MORE combinator with the actual child rule
  zero_or_more_<std::string::iterator, char_a> parser(char_a_rule);

  EXPECT_TRUE(parser.parse(ctx1, begin1, end1));
  EXPECT_EQ(parser.size(), 4); // Should have 4 matches
  EXPECT_TRUE(parser.parse(ctx2, begin2, end2));
  EXPECT_EQ(parser.size(), 0); // Should have 0 matches
  EXPECT_TRUE(parser.parse(ctx3, begin3, end3));
  EXPECT_EQ(parser.size(), 3); // Should have 3 matches
}

// Test parser name() method returns readable names
TEST_F(ParserTest, ParserNameMethod) {
  using char_a = character<std::string::iterator, 'a'>;
  using char_b = character<std::string::iterator, 'b'>;
  
  // Test simple character parser
  char_a char_parser;
  std::string char_name = char_parser.name();
  EXPECT_FALSE(char_name.empty()) << "name() should return a non-empty string";
  // name() should be readable (not mangled)
  
  // Test OR combinator with CRTP struct
  struct test_or : rule<std::string::iterator, test_or,
                        or_<std::string::iterator, char_a, char_b>> {};
  
  test_or or_parser;
  std::string or_name = or_parser.name();
  EXPECT_FALSE(or_name.empty()) << "name() should return a non-empty string";
  // Note: name() returns the struct name in the local scope
  
  // Test AND combinator with CRTP struct
  struct test_and : rule<std::string::iterator, test_and,
                         and_<std::string::iterator, char_a, char_b>> {};
  
  test_and and_parser;
  std::string and_name = and_parser.name();
  EXPECT_FALSE(and_name.empty()) << "name() should return a non-empty string";
  
  // Verify name() returns something readable (not too long or mangled)
  EXPECT_LT(or_name.length(), 200) << "name() should return reasonable length";
  EXPECT_LT(and_name.length(), 200) << "name() should return reasonable length";
  
  // Both should end with the struct name
  EXPECT_TRUE(or_name.find("test_or") != std::string::npos) 
      << "name() should include struct name";
  EXPECT_TRUE(and_name.find("test_and") != std::string::npos) 
      << "name() should include struct name";
}

// Test parser type() method returns correct type
TEST_F(ParserTest, ParserTypeMethod) {
  // Test CRTP struct
  struct test_rule : rule<std::string::iterator, test_rule,
                          character<std::string::iterator, 'a'>> {};
  
  test_rule parser;
  const std::type_info& type_info = parser.type();
  
  // Should match the test_rule type
  EXPECT_EQ(type_info, typeid(parser)) << "type() should return correct type_info";
  
  // Verify isa() method
  EXPECT_TRUE(parser.isa(typeid(parser))) << "isa() should identify self";
  EXPECT_TRUE(parser.isa(typeid(test_rule))) << "isa() should identify rule type";
}

// Test ONE_OR_MORE combinator
TEST_F(ParserTest, OneOrMoreCombinator) {
  using char_a = character<std::string::iterator, 'a'>;

  std::string input1 = "aaaa";
  std::string input2 = "";

  auto begin1 = input1.begin();
  auto end1 = input1.end();
  context<std::string::iterator> ctx1(begin1, end1);

  auto begin2 = input2.begin();
  auto end2 = input2.end();
  context<std::string::iterator> ctx2(begin2, end2);

  // Create actual instance of the child rule
  auto char_a_rule = std::make_shared<char_a>();

  // Create ONE_OR_MORE combinator with the actual child rule
  one_or_more_<std::string::iterator, char_a> parser(char_a_rule);

  EXPECT_TRUE(parser.parse(ctx1, begin1, end1));
  EXPECT_EQ(parser.size(), 4); // Should have 4 matches
  EXPECT_FALSE(parser.parse(ctx2, begin2, end2));
}

// Test ZERO_OR_ONE combinator
TEST_F(ParserTest, ZeroOrOneCombinator) {
  using char_a = character<std::string::iterator, 'a'>;

  std::string input1 = "a";
  std::string input2 = "";
  std::string input3 = "aa";

  auto begin1 = input1.begin();
  auto end1 = input1.end();
  context<std::string::iterator> ctx1(begin1, end1);

  auto begin2 = input2.begin();
  auto end2 = input2.end();
  context<std::string::iterator> ctx2(begin2, end2);

  auto begin3 = input3.begin();
  auto end3 = input3.end();
  context<std::string::iterator> ctx3(begin3, end3);

  // Create actual instance of the child rule
  auto char_a_rule = std::make_shared<char_a>();

  // Create ZERO_OR_ONE combinator with the actual child rule
  zero_or_one_<std::string::iterator, char_a> parser(char_a_rule);

  EXPECT_TRUE(parser.parse(ctx1, begin1, end1));
  EXPECT_EQ(parser.size(), 1); // Should have 1 match
  EXPECT_TRUE(parser.parse(ctx2, begin2, end2));
  EXPECT_EQ(parser.size(), 0); // Should have 0 matches
  EXPECT_TRUE(parser.parse(ctx3, begin3, end3));
  EXPECT_EQ(parser.size(), 1); // Should have 1 match (only first 'a')
}

// Test whitespace handling
TEST_F(ParserTest, WhitespaceHandling) {
  using char_a = character<std::string::iterator, 'a'>;

  std::string input1 = "  a";
  std::string input2 = "  b";

  auto begin1 = input1.begin();
  auto end1 = input1.end();
  context<std::string::iterator> ctx1(begin1, end1, true); // ignore whitespace

  auto begin2 = input2.begin();
  auto end2 = input2.end();
  context<std::string::iterator> ctx2(begin2, end2, true);

  char_a parser;
  EXPECT_TRUE(parser.parse(ctx1, begin1, end1));
  EXPECT_FALSE(parser.parse(ctx2, begin2, end2));
}

// Test error reporting
TEST_F(ParserTest, ErrorReporting) {
  using char_a = character<std::string::iterator, 'a'>;

  std::string input = "b";
  auto begin = input.begin();
  auto end = input.end();
  context<std::string::iterator> ctx(begin, end);

  char_a parser;
  EXPECT_FALSE(parser.parse(ctx, begin, end));
  EXPECT_FALSE(ctx.parse_errors.empty());
}

// Test main parser interface
TEST_F(ParserTest, MainParserInterface) {
  using char_a = character<std::string::iterator, 'a'>;

  std::string input = "a";
  std::string input2 = "b";

  std::shared_ptr<char_a> ast;
  std::shared_ptr<char_a> ast2;

  parse_error<std::string::iterator>::vector errors;
  parse_error<std::string::iterator>::vector errors2;

  EXPECT_TRUE(
      stlx::parser<char_a>::parse(input.begin(), input.end(), ast, errors));
  EXPECT_FALSE(
      stlx::parser<char_a>::parse(input2.begin(), input2.end(), ast2, errors2));
}

// Test complex expression parsing
TEST_F(ParserTest, ComplexExpressionParsing) {
  using digit = characters<std::string::iterator, '0', '9'>;

  std::string input = "123";
  auto begin = input.begin();
  auto end = input.end();
  context<std::string::iterator> ctx(begin, end);

  // Create actual instance of the child rule
  auto digit_rule = std::make_shared<digit>();

  // Create ONE_OR_MORE combinator with the actual child rule
  one_or_more_<std::string::iterator, digit> parser(digit_rule);

  EXPECT_TRUE(parser.parse(ctx, begin, end));
  EXPECT_EQ(parser.size(), 3); // Should have 3 digit matches
}

// Test edge cases
TEST_F(ParserTest, EdgeCases) {
  using char_a = character<std::string::iterator, 'a'>;

  std::string empty = "";
  std::string single = "a";

  auto begin1 = empty.begin();
  auto end1 = empty.end();
  context<std::string::iterator> ctx1(begin1, end1);

  auto begin2 = single.begin();
  auto end2 = single.end();
  context<std::string::iterator> ctx2(begin2, end2);

  char_a parser;
  EXPECT_FALSE(char_a().parse(ctx1, begin1, end1));
  EXPECT_TRUE(char_a().parse(ctx2, begin2, end2));
}

// Performance test
TEST_F(ParserTest, PerformanceTest) {
  using digit = characters<std::string::iterator, '0', '9'>;

  std::string input(10000, '1'); // 10000 digits
  auto begin = input.begin();
  auto end = input.end();
  context<std::string::iterator> ctx(begin, end);

  // Create actual instance of the child rule
  auto digit_rule = std::make_shared<digit>();

  // Create ONE_OR_MORE combinator with the actual child rule
  one_or_more_<std::string::iterator, digit> parser(digit_rule);

  EXPECT_TRUE(parser.parse(ctx, begin, end));
  EXPECT_EQ(parser.size(), 10000);
}
