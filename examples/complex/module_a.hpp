/**
 * @file module_a.hpp
 * @brief Module A header for complex example application
 *
 * This header defines the ModuleA class that demonstrates various
 * STLX library features and C++23 concepts.
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef COMPLEX_EXAMPLE_MODULE_A_HPP
#define COMPLEX_EXAMPLE_MODULE_A_HPP

#include <memory>
#include <stlx/stlx.hpp>
#include <string>
#include <vector>

namespace complex_example {

class ModuleA {
public:
  explicit ModuleA(const std::string &name);
  ~ModuleA() = default;

  void process_data(const std::vector<int> &data);
  std::string get_status() const;

  // C++23 features
  void demonstrate_concepts();
  void demonstrate_ranges();

private:
  std::string name_;
  std::vector<int> processed_data_;
  mutable std::size_t access_count_{0};
};

// Factory function using C++23 features
std::unique_ptr<ModuleA> create_module(const std::string &name);

} // namespace complex_example

#endif // COMPLEX_EXAMPLE_MODULE_A_HPP
