/**
 * @file test_simple.cpp
 * @brief Simple unit tests for STLX library
 *
 * This file contains basic unit tests to verify STLX library functionality
 * and ensure proper integration with the testing framework.
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#include "stlx/stlx.hpp"
#include "stlx/xstring.hpp"
#include "gtest/gtest.h"

using namespace stlx;

TEST(BasicTest, HelloWorld) { EXPECT_EQ(1 + 1, 2); }

TEST(STLXTest, PrintString) {
  print_string("Hello, World!");
  EXPECT_TRUE(true);
}

TEST(XStringSimpleTest, BasicFunctionality) {
  string s("Hello");
  EXPECT_EQ(s, "Hello");
  EXPECT_TRUE(s.ends_with("lo"));
}
