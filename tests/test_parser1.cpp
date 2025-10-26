/**
 * @file test_parser1.cpp
 * @brief Unit tests for parser1 grammar (command-line parser example)
 *
 * This file contains comprehensive unit tests for the parser1 grammar which
 * demonstrates parsing command-line arguments for color and prime parameters.
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#include <gtest/gtest.h>
#include "stlx/parser.hpp"
#include <iostream>

// Include the parser1 grammar definitions
namespace command_line {
using namespace stlx::parse;

// Terminals
STRING(red, "red");
STRING(green, "green");
STRING(blue, "blue");
STRING(one, "1");
STRING(three, "3");
STRING(five, "5");
STRING(dash_color, "--color=");
STRING(dash_prime, "--prime=");

// Rules
using rgb = or_<std::string::const_iterator, red, green, blue>;
using prime_num = or_<std::string::const_iterator, one, three, five>;
using color_param = and_<std::string::const_iterator, dash_color, rgb>;
using prime_param = and_<std::string::const_iterator, dash_prime, prime_num>;
using parameter = or_<std::string::const_iterator, color_param, prime_param>;
} // namespace command_line

class Parser1Test : public ::testing::Test {
protected:
  void SetUp() override {
    // Common setup if needed
  }

  void TearDown() override {
    // Common cleanup if needed
  }
};

// Test valid color parameters
TEST_F(Parser1Test, ValidColorParameters) {
  std::string inputs[] = {"--color=red", "--color=green", "--color=blue"};
  
  for (const auto &input : inputs) {
    std::shared_ptr<command_line::parameter> ast;
    using parser_t = stlx::parser<command_line::parameter>;
    
    bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
    EXPECT_TRUE(success) << "Failed to parse: " << input;
    EXPECT_NE(ast, nullptr) << "AST is null for: " << input;
  }
}

// Test invalid color parameters
TEST_F(Parser1Test, InvalidColorParameters) {
  std::string inputs[] = {"--color=yellow", "--color=blueeee", "--color=", "--color"};
  
  for (const auto &input : inputs) {
    std::shared_ptr<command_line::parameter> ast;
    using parser_t = stlx::parser<command_line::parameter>;
    
    bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
    EXPECT_FALSE(success) << "Should have failed to parse: " << input;
  }
}

// Test valid prime parameters
TEST_F(Parser1Test, ValidPrimeParameters) {
  std::string inputs[] = {"--prime=1", "--prime=3", "--prime=5"};
  
  for (const auto &input : inputs) {
    std::shared_ptr<command_line::parameter> ast;
    using parser_t = stlx::parser<command_line::parameter>;
    
    bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
    EXPECT_TRUE(success) << "Failed to parse: " << input;
    EXPECT_NE(ast, nullptr) << "AST is null for: " << input;
  }
}

// Test invalid prime parameters
TEST_F(Parser1Test, InvalidPrimeParameters) {
  std::string inputs[] = {"--prime=2", "--prime=4", "--prime=7", "--prime="};
  
  for (const auto &input : inputs) {
    std::shared_ptr<command_line::parameter> ast;
    using parser_t = stlx::parser<command_line::parameter>;
    
    bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
    EXPECT_FALSE(success) << "Should have failed to parse: " << input;
  }
}

// Test color parameter parsing with error details
TEST_F(Parser1Test, ColorParameterWithErrors) {
  std::string input = "--color=yellow";
  std::shared_ptr<command_line::parameter> ast;
  using parser_t = stlx::parser<command_line::parameter>;
  stlx::parse::parse_error<std::string::const_iterator>::vector errors;
  
  bool success = parser_t::parse(input.cbegin(), input.cend(), ast, errors);
  
  EXPECT_FALSE(success);
  EXPECT_FALSE(errors.empty()) << "Should have error details";
  
  // Check that error positions are valid
  for (const auto &err : errors) {
    EXPECT_GE(err->position, input.cbegin());
    EXPECT_LE(err->position, input.cend());
  }
}

// Test prime parameter parsing with error details
TEST_F(Parser1Test, PrimeParameterWithErrors) {
  std::string input = "--prime=2";
  std::shared_ptr<command_line::parameter> ast;
  using parser_t = stlx::parser<command_line::parameter>;
  stlx::parse::parse_error<std::string::const_iterator>::vector errors;
  
  bool success = parser_t::parse(input.cbegin(), input.cend(), ast, errors);
  
  EXPECT_FALSE(success);
  EXPECT_FALSE(errors.empty()) << "Should have error details";
}

// Test that partial matches fail
TEST_F(Parser1Test, PartialMatchesFail) {
  std::string inputs[] = {"--color", "--prime", "red", "blue", "1", "3"};
  
  for (const auto &input : inputs) {
    std::shared_ptr<command_line::parameter> ast;
    using parser_t = stlx::parser<command_line::parameter>;
    
    bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
    EXPECT_FALSE(success) << "Should have failed to parse: " << input;
  }
}

// Test empty input fails
TEST_F(Parser1Test, EmptyInputFails) {
  std::string input = "";
  std::shared_ptr<command_line::parameter> ast;
  using parser_t = stlx::parser<command_line::parameter>;
  
  bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
  EXPECT_FALSE(success) << "Should fail on empty input";
}

// Test that extraneous content fails
TEST_F(Parser1Test, ExtraneousContentFails) {
  std::string inputs[] = {"--color=red extra", "--prime=3 extra"};
  
  for (const auto &input : inputs) {
    std::shared_ptr<command_line::parameter> ast;
    using parser_t = stlx::parser<command_line::parameter>;
    
    bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
    EXPECT_FALSE(success) << "Should fail with extraneous content: " << input;
  }
}

// Test AST structure for color parameter
TEST_F(Parser1Test, ColorParameterASTStructure) {
  std::string input = "--color=red";
  std::shared_ptr<command_line::parameter> ast;
  using parser_t = stlx::parser<command_line::parameter>;
  
  bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
  
  EXPECT_TRUE(success);
  ASSERT_NE(ast, nullptr);
  EXPECT_EQ(ast->size(), 1) << "Color parameter should have one child";
}

// Test AST structure for prime parameter
TEST_F(Parser1Test, PrimeParameterASTStructure) {
  std::string input = "--prime=5";
  std::shared_ptr<command_line::parameter> ast;
  using parser_t = stlx::parser<command_line::parameter>;
  
  bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
  
  EXPECT_TRUE(success);
  ASSERT_NE(ast, nullptr);
  EXPECT_EQ(ast->size(), 1) << "Prime parameter should have one child";
}

// Test multiple parsing attempts
TEST_F(Parser1Test, MultipleParsingAttempts) {
  std::string inputs[] = {"--color=blue", "--prime=1", "--color=green", "--prime=3"};
  
  for (const auto &input : inputs) {
    std::shared_ptr<command_line::parameter> ast;
    using parser_t = stlx::parser<command_line::parameter>;
    
    bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
    EXPECT_TRUE(success) << "Failed to parse: " << input;
    EXPECT_NE(ast, nullptr) << "AST is null for: " << input;
  }
}

