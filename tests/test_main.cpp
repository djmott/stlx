/**
 * @file test_main.cpp
 * @brief Main test entry point for STLX library tests
 *
 * This file provides the main entry point for running all STLX library tests
 * using Google Test framework.
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#include <gtest/gtest.h>

// Main test file - this will be linked with gtest_main
// Additional test files can be added here

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
