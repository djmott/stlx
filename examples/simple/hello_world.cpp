#include <iostream>
#include <stlx/stlx.hpp>
#include <string>
#include <vector>

int main() {
  using namespace stlx;

  // Demonstrate C++23 features and STLX utilities

  // String utilities
  std::string text = "  Hello, C++23 World!  ";
  std::cout << "Original: '" << text << "'\n";
  std::cout << "Trimmed: '" << trim(text) << "'\n";

  // String checking
  std::string filename = "example.cpp";
  std::cout << "Filename '" << filename
            << "' ends with .cpp: " << std::boolalpha
            << ends_with(filename, ".cpp") << "\n";

  // Numeric utilities
  int value = 15;
  std::cout << "Value " << value << " in range [10, 20]: " << std::boolalpha
            << in_range(value, 10, 20) << "\n";

  int clamped = clamp(value, 0, 10);
  std::cout << "Clamped " << value << " to [0, 10]: " << clamped << "\n";

  // Format utilities
  std::string formatted =
      format_string("Hello {}! You have {} items.", "World", 42);
  std::cout << "Formatted: " << formatted << "\n";

  // Print utilities (C++23 std::print if available)
  print_line("Using stlx::print_line: {} items found", 123);

  // Compile-time string hashing
  constexpr std::size_t hash = hash_string("Hello World");
  std::cout << "Hash of 'Hello World': " << hash << "\n";

  // RAII scope exit
  {
    auto cleanup =
        make_scope_exit([]() { std::cout << "Cleanup executed!\n"; });
    std::cout << "Inside scope, cleanup will be called when leaving...\n";
  }

  std::cout << "Hello World example completed successfully!\n";
  return 0;
}
