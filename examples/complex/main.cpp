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
