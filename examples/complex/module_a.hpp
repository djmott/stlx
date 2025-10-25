#pragma once

#include <stlx/stlx.hpp>
#include <string>
#include <vector>
#include <memory>

namespace complex_example {

class ModuleA {
public:
    explicit ModuleA(const std::string& name);
    ~ModuleA() = default;
    
    void process_data(const std::vector<int>& data);
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
std::unique_ptr<ModuleA> create_module(const std::string& name);

} // namespace complex_example
