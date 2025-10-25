/**
 * @file main.cpp
 * @brief Complex example application demonstrating STLX library usage
 *
 * This example shows how to use the STLX library in a multi-file application
 * with separate compilation units and demonstrates various C++23 features.
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#include "module_a.hpp"
#include <exception>
#include <iostream>
#include <vector>

int main() {
  using namespace complex_example;

  std::cout << "=== Complex Example Application ===\n\n";

  try {
    // Create module using factory function
    auto module = create_module("DataProcessor");

    // Process some data
    std::vector<int> test_data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    module->process_data(test_data);

    // Get status
    std::cout << module->get_status() << "\n\n";

    // Demonstrate C++23 features
    module->demonstrate_concepts();
    std::cout << "\n";
    module->demonstrate_ranges();
    std::cout << "\n";

    // Demonstrate RAII with scope_exit
    {
      auto cleanup = stlx::make_scope_exit(
          [&module]() { std::cout << "Cleaning up module resources...\n"; });

      std::cout << "Working with module inside scope...\n";
      std::cout << "Final status: " << module->get_status() << "\n";
    }

    // Test error handling
    try {
      auto empty_module = create_module("   "); // Should throw
    } catch (const std::exception &e) {
      std::cout << "Caught expected exception: " << e.what() << "\n";
    }

    std::cout << "\nComplex example completed successfully!\n";

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
