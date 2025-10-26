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

#include "stlx/parser.hpp"
#include <gtest/gtest.h>
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

// Rules - using CRTP-based struct pattern
struct rgb : rule<std::string::const_iterator, rgb,
                  or_<std::string::const_iterator, red, green, blue>> {};

struct prime_num : rule<std::string::const_iterator, prime_num,
                        or_<std::string::const_iterator, one, three, five>> {};

struct color_param : rule<std::string::const_iterator, color_param,
                          and_<std::string::const_iterator, dash_color, rgb>> {};

struct prime_param : rule<std::string::const_iterator, prime_param,
                          and_<std::string::const_iterator, dash_prime, prime_num>> {};

struct parameter : rule<std::string::const_iterator, parameter,
                        or_<std::string::const_iterator, color_param, prime_param>> {};
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
    // Create mutable copy for parsing
    
    std::shared_ptr<command_line::parameter> ast;
    using parser_t = stlx::parser<command_line::parameter>;

    bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
    EXPECT_TRUE(success) << "Failed to parse: " << input;
    EXPECT_NE(ast, nullptr) << "AST is null for: " << input;
    
    // Verify AST type
    EXPECT_TRUE(ast->isa(typeid(command_line::parameter)))
        << "AST should be identified as parameter";
  }
}

// Test rule type information using CRTP
TEST_F(Parser1Test, RuleTypeInformation) {
  // Test rgb rule
  std::string input = "red";
  stlx::parse::context<std::string::const_iterator> ctx(input.cbegin(), input.cend());
  std::string::const_iterator begin = input.cbegin();
  std::string::const_iterator end = input.cend();
  
  command_line::rgb rgb_rule;
  bool rgb_success = rgb_rule.parse(ctx, begin, end);
  EXPECT_TRUE(rgb_success);
  
  const std::type_info& rgb_type = rgb_rule.type();
  EXPECT_EQ(rgb_type, typeid(command_line::rgb))
      << "rgb rule type should be command_line::rgb";
  
  EXPECT_TRUE(rgb_rule.isa(typeid(command_line::rgb)))
      << "rgb rule should be identified as command_line::rgb";
  
  std::string rgb_name = rgb_rule.name();
  EXPECT_FALSE(rgb_name.empty()) << "rgb name() should return non-empty string";
  EXPECT_TRUE(rgb_name.find("rgb") != std::string::npos)
      << "rgb name() should contain 'rgb'";
  
  // Test prime_num rule
  std::string prime_input = "5";
  stlx::parse::context<std::string::const_iterator> prime_ctx(prime_input.cbegin(), prime_input.cend());
  std::string::const_iterator prime_begin = prime_input.cbegin();
  std::string::const_iterator prime_end = prime_input.cend();
  
  command_line::prime_num prime_rule;
  bool prime_success = prime_rule.parse(prime_ctx, prime_begin, prime_end);
  EXPECT_TRUE(prime_success);
  
  const std::type_info& prime_type = prime_rule.type();
  EXPECT_EQ(prime_type, typeid(command_line::prime_num))
      << "prime_num rule type should be command_line::prime_num";
  
  EXPECT_TRUE(prime_rule.isa(typeid(command_line::prime_num)))
      << "prime_num rule should be identified as command_line::prime_num";
  
  std::string prime_name = prime_rule.name();
  EXPECT_FALSE(prime_name.empty()) << "prime_num name() should return non-empty string";
  EXPECT_TRUE(prime_name.find("prime") != std::string::npos)
      << "prime_num name() should contain 'prime'";
}

// Test invalid color parameters
TEST_F(Parser1Test, InvalidColorParameters) {
  std::string inputs[] = {"--color=yellow", "--color=blueeee",
                          "--color=", "--color"};

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
    
    // Verify AST type
    EXPECT_TRUE(ast->isa(typeid(command_line::parameter)))
        << "AST should be identified as parameter";
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
  
  // Test type() method
  const std::type_info& type_info = ast->type();
  EXPECT_EQ(type_info, typeid(command_line::parameter))
      << "AST type should be command_line::parameter";
  
  // Test isa() method
  EXPECT_TRUE(ast->isa(typeid(command_line::parameter)))
      << "AST should be identified as parameter";
  
  // Test name() method
  std::string ast_name = ast->name();
  EXPECT_FALSE(ast_name.empty()) << "name() should return non-empty string";
  EXPECT_TRUE(ast_name.find("parameter") != std::string::npos)
      << "name() should contain 'parameter'";
  
  // Check child type for color_param
  if (ast->size() > 0 && (*ast)[0]) {
    const std::type_info& child_type = (*ast)[0]->type();
    EXPECT_EQ(child_type, typeid(command_line::color_param))
        << "Child type should be color_param";
    
    EXPECT_TRUE((*ast)[0]->isa(typeid(command_line::color_param)))
        << "Child should be identified as color_param";
  }
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
  
  // Test type() method
  const std::type_info& type_info = ast->type();
  EXPECT_EQ(type_info, typeid(command_line::parameter))
      << "AST type should be command_line::parameter";
  
  // Test isa() method
  EXPECT_TRUE(ast->isa(typeid(command_line::parameter)))
      << "AST should be identified as parameter";
  
  // Test name() method
  std::string ast_name = ast->name();
  EXPECT_FALSE(ast_name.empty()) << "name() should return non-empty string";
  EXPECT_TRUE(ast_name.find("parameter") != std::string::npos)
      << "name() should contain 'parameter'";
  
  // Check child type for prime_param
  if (ast->size() > 0 && (*ast)[0]) {
    const std::type_info& child_type = (*ast)[0]->type();
    EXPECT_EQ(child_type, typeid(command_line::prime_param))
        << "Child type should be prime_param";
    
    EXPECT_TRUE((*ast)[0]->isa(typeid(command_line::prime_param)))
        << "Child should be identified as prime_param";
  }
}

// Test multiple parsing attempts
TEST_F(Parser1Test, MultipleParsingAttempts) {
  std::string inputs[] = {"--color=blue", "--prime=1", "--color=green",
                          "--prime=3"};

  for (const auto &input : inputs) {
    std::shared_ptr<command_line::parameter> ast;
    using parser_t = stlx::parser<command_line::parameter>;

    bool success = parser_t::parse(input.cbegin(), input.cend(), ast);
    EXPECT_TRUE(success) << "Failed to parse: " << input;
    EXPECT_NE(ast, nullptr) << "AST is null for: " << input;
  }
}
