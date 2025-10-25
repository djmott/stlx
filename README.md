# STLX - C++23 Utilities and Examples

[![CI/CD Pipeline](https://github.com/djmott/stlx/actions/workflows/ci.yml/badge.svg)](https://github.com/djmott/stlx/actions/workflows/ci.yml)
[![Pull Request Checks](https://github.com/djmott/stlx/actions/workflows/pr.yml/badge.svg)](https://github.com/djmott/stlx/actions/workflows/pr.yml)
[![Release Pipeline](https://github.com/djmott/stlx/actions/workflows/release.yml/badge.svg)](https://github.com/djmott/stlx/actions/workflows/release.yml)
[![Scheduled Analysis](https://github.com/djmott/stlx/actions/workflows/scheduled.yml/badge.svg)](https://github.com/djmott/stlx/actions/workflows/scheduled.yml)

A comprehensive CMake project demonstrating modern C++23 features with integrated testing, sanitizers, static/dynamic analysis tools, and code coverage support.

## CI/CD Pipeline

This project includes a comprehensive GitHub Actions CI/CD pipeline that provides:

### 🔄 Continuous Integration
- **Multi-Compiler Testing**: GCC 11-13, Clang 14-17, MSVC 2019/2022
- **Multi-Platform Support**: Ubuntu, Windows, macOS
- **Sanitizer Testing**: Address, UndefinedBehavior, Thread, Leak, Memory sanitizers
- **Static Analysis**: cppcheck, cpplint, clang-tidy, IWYU
- **Dynamic Analysis**: Valgrind memory leak detection
- **Code Coverage**: HTML reports with gcovr

### 📊 Analysis Reports
- **Test Results**: XML and TXT format reports
- **Coverage Reports**: Interactive HTML coverage reports
- **Static Analysis**: Comprehensive static analysis results
- **Performance Benchmarks**: Performance trend analysis
- **Security Scans**: Vulnerability scanning with Trivy

### 📦 Artifact Downloads
All analysis reports and coverage data are available as downloadable artifacts:
- Test results from all configurations
- Coverage reports in HTML format
- Static analysis reports (XML/TXT)
- Performance benchmark data
- Security scan results

### 🎯 Workflow Matrix
The CI pipeline tests **174+ build configurations**:
- 63 GCC configurations (3 versions × 7 sanitizers × 3 configs)
- 84 Clang configurations (4 versions × 7 sanitizers × 3 configs)
- 12 MSVC configurations (2 versions × 3 sanitizers × 2 configs)
- 15 macOS configurations (1 platform × 5 sanitizers × 3 configs)

## Features

- **C++23 Standard**: Full C++23 support with modern language features
- **Header-Only Library**: STLX utilities library with constexpr functions and concepts
- **Sample Applications**: Both simple and complex examples demonstrating library usage
- **Comprehensive Testing**: GTest-based test suite with CTest integration
- **Sanitizer Support**: Address, Undefined Behavior, Thread, Leak, and Memory sanitizers
- **Static Analysis**: Integration with clang-tidy and cppcheck
- **Dynamic Analysis**: Valgrind memory leak detection
- **Code Coverage**: Support for both gcovr and lcov with HTML reports
- **Code Formatting**: clang-format integration
- **Optional Tools**: All analysis tools are optional - project builds even if tools are missing

## Project Structure

```
stlx/
├── CMakeLists.txt              # Root CMake configuration
├── cmake_modules/              # Custom CMake find modules
│   ├── Findclang-tidy.cmake
│   ├── Findclang-format.cmake
│   ├── Findcppcheck.cmake
│   ├── Findvalgrind.cmake
│   ├── Findgcovr.cmake
│   └── Sanitizers.cmake
├── include/stlx/               # Header-only library
│   └── stlx.hpp
├── examples/                   # Sample applications
│   ├── simple/
│   │   └── hello_world.cpp
│   └── complex/
│       ├── main.cpp
│       ├── module_a.cpp
│       └── module_a.hpp
├── tests/                      # Test suite
│   ├── test_main.cpp
│   └── test_stlx.cpp
└── README.md
```

## Building the Project

### Basic Build

```bash
mkdir build
cd build
cmake ..
make
```

### Build with Sanitizers

```bash
mkdir build-sanitized
cd build-sanitized
cmake -DSANITIZER_TYPE=Address ..
make
```

Available sanitizer options:
- `None` (default)
- `Address`
- `UndefinedBehavior`
- `Thread`
- `Leak`
- `Memory`
- `All`

### Build with Code Coverage

```bash
mkdir build-coverage
cd build-coverage
cmake -DENABLE_COVERAGE=ON ..
make
make coverage_html  # Generate HTML coverage report
```

### Build with Static Analysis

```bash
mkdir build-analysis
cd build-analysis
cmake -DENABLE_CLANG_TIDY=ON -DENABLE_CPPCHECK=ON ..
make
make cppcheck  # Run cppcheck analysis
```

## Running Tests

### Basic Test Execution

```bash
cd build
make test
# or
ctest
```

### Run Tests with Valgrind

```bash
cd build
make valgrind-tests
```

### Run Tests with Specific Sanitizer

```bash
mkdir build-ubsan
cd build-ubsan
cmake -DSANITIZER_TYPE=UndefinedBehavior ..
make
./bin/stlx_tests
```

## Available Targets

### Build Targets
- `hello_world` - Simple example application
- `complex_example` - Multi-file example application
- `stlx_tests` - Test suite executable

### Analysis Targets
- `cppcheck` - Run cppcheck static analysis (if available)
- `format` - Format code with clang-format (if available)
- `valgrind-tests` - Run tests with valgrind (if available)

### Coverage Targets
- `coverage_raw` - Generate raw coverage data
- `coverage_html` - Generate HTML coverage report (requires gcovr)
- `coverage` - Generate both raw and HTML reports

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `SANITIZER_TYPE` | `None` | Sanitizer to enable |
| `ENABLE_COVERAGE` | `OFF` | Enable code coverage |
| `ENABLE_CLANG_TIDY` | `OFF` | Enable clang-tidy analysis |
| `ENABLE_CPPCHECK` | `OFF` | Enable cppcheck analysis |

## STLX Library Features

The STLX header-only library provides:

### String Utilities
- `trim()`, `trim_left()`, `trim_right()` - String trimming
- `starts_with()`, `ends_with()`, `contains()` - String checking
- `to_string_view()` - Safe string view conversion

### Numeric Utilities
- `clamp()` - Value clamping
- `in_range()` - Range checking

### Format Utilities
- `format_string()` - Type-safe string formatting
- `print_string()`, `print_line()` - Print utilities with C++23 std::print support

### RAII Utilities
- `make_scope_exit()` - Scope-based cleanup

### Compile-time Features
- `hash_string()` - Compile-time string hashing
- Concepts for type safety (`StringLike`, `Numeric`, `ScopedEnum`)

## Example Usage

```cpp
#include <stlx/stlx.hpp>

int main() {
    using namespace stlx;
    
    // String utilities
    std::string text = "  Hello, World!  ";
    std::cout << trim(text) << "\n";  // "Hello, World!"
    
    // Numeric utilities
    int value = clamp(15, 0, 10);     // 10
    bool in_range = in_range(5, 0, 10); // true
    
    // Format utilities
    std::string formatted = format_string("Value: {}", 42);
    
    // RAII cleanup
    {
        auto cleanup = make_scope_exit([]() {
            std::cout << "Cleanup!\n";
        });
        // ... work ...
    } // cleanup called automatically
}
```

## Requirements

### Compiler
- GCC 11+ or Clang 14+ with C++23 support
- CMake 3.20+

### Optional Tools
- clang-tidy (for static analysis)
- clang-format (for code formatting)
- cppcheck (for static analysis)
- valgrind (for dynamic analysis)
- gcovr or lcov (for coverage reports)

## Contributing

1. Ensure code follows C++23 standards
2. Add tests for new functionality
3. Run static analysis tools before submitting
4. Format code with clang-format

## License

This project is provided as an educational example of modern C++23 development practices.
