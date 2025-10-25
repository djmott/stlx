#include "module_a.hpp"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>

namespace complex_example {

ModuleA::ModuleA(const std::string &name) : name_(name) {
  std::cout << "ModuleA '" << name_ << "' constructed\n";
}

void ModuleA::process_data(const std::vector<int> &data) {
  processed_data_.clear();
  processed_data_.reserve(data.size());

  // Use C++23 ranges and algorithms
  std::ranges::transform(data, std::back_inserter(processed_data_),
                         [](int value) { return value * 2; });

  std::cout << "Processed " << data.size() << " items in ModuleA '" << name_
            << "'\n";
}

std::string ModuleA::get_status() const {
  ++access_count_;

  return stlx::format_string(
      "ModuleA '{}': {} items processed, accessed {} times", name_,
      processed_data_.size(), access_count_);
}

void ModuleA::demonstrate_concepts() {
  std::cout << "=== Demonstrating C++23 Concepts ===\n";

  // String utilities
  std::string test_str = "  C++23 Concepts Demo  ";
  std::cout << "Trimmed: '" << stlx::trim(test_str) << "'\n";

  // Numeric utilities
  std::vector<int> numbers = {1, 5, 10, 15, 20};
  for (int n : numbers) {
    std::cout << "Value " << n << " in range [5, 15]: " << std::boolalpha
              << stlx::in_range(n, 5, 15) << "\n";
  }

  // Compile-time hashing
  constexpr auto hash1 = stlx::hash_string("test1");
  constexpr auto hash2 = stlx::hash_string("test2");
  std::cout << "Hash 'test1': " << hash1 << "\n";
  std::cout << "Hash 'test2': " << hash2 << "\n";
}

void ModuleA::demonstrate_ranges() {
  std::cout << "=== Demonstrating C++23 Ranges ===\n";

  std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  // Filter even numbers and square them
  auto even_squares = data |
                      std::views::filter([](int n) { return n % 2 == 0; }) |
                      std::views::transform([](int n) { return n * n; });

  std::cout << "Even squares: ";
  for (int value : even_squares) {
    std::cout << value << " ";
  }
  std::cout << "\n";

  // Sum of processed data
  if (!processed_data_.empty()) {
    int sum =
        std::accumulate(processed_data_.begin(), processed_data_.end(), 0);
    std::cout << "Sum of processed data: " << sum << "\n";
  }
}

std::unique_ptr<ModuleA> create_module(const std::string &name) {
  if (stlx::trim(name).empty()) {
    throw std::invalid_argument("Module name cannot be empty");
  }

  return std::make_unique<ModuleA>(name);
}

} // namespace complex_example
