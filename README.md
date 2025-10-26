# STLX - C++23 Utilities and Examples

[![CI/CD Pipeline](https://github.com/djmott/stlx/actions/workflows/ci.yml/badge.svg)](https://github.com/djmott/stlx/actions/workflows/ci.yml)
[![Pull Request Checks](https://github.com/djmott/stlx/actions/workflows/pr.yml/badge.svg)](https://github.com/djmott/stlx/actions/workflows/pr.yml)
[![Release Pipeline](https://github.com/djmott/stlx/actions/workflows/release.yml/badge.svg)](https://github.com/djmott/stlx/actions/workflows/release.yml)
[![Scheduled Analysis](https://github.com/djmott/stlx/actions/workflows/scheduled.yml/badge.svg)](https://github.com/djmott/stlx/actions/workflows/scheduled.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/59354c5ee86e4e1c969ea5cbbe59d9c1)](https://app.codacy.com/gh/djmott/stlx/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

A comprehensive CMake project demonstrating modern C++23 features with integrated testing, sanitizers, static/dynamic analysis tools, and code coverage support.

## Quick Start

### Prerequisites
- C++23 compatible compiler (GCC 11+, Clang 14+, MSVC 19.29+)
- CMake 3.20 or higher
- Ninja build system (recommended) or Make

### Installation
```bash
git clone https://github.com/djmott/stlx.git
cd stlx
mkdir build && cd build
cmake -GNinja ..
ninja
ninja test
```

### Usage
```cpp
#include <stlx/stlx.hpp>
#include <stlx/parser.hpp>      // For parser combinators
#include <stlx/xstring.hpp>    // For extended string utilities
#include <stlx/grammars/abasic.hpp>  // For BASIC grammar

using namespace stlx;

int main() {
    // Use STLX utilities
    std::string text = "  Hello World  ";
    auto trimmed = trim(text);
    
    // Use parser combinators
    using namespace stlx::parse;
    character<char*> comma(',');
    // ... parser usage ...
    
    return 0;
}
```

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
│   ├── FindCLANG_TIDY.cmake
│   ├── FindCLANG_FORMAT.cmake
│   ├── FindCPPCHECK.cmake
│   ├── FindCPPLINT.cmake
│   ├── FindGCOVR.cmake
│   ├── FindIWYU.cmake
│   ├── FindVALGRIND.cmake
│   └── Sanitizers.cmake
├── include/stlx/               # Header-only library
│   ├── stlx.hpp               # Main utilities header
│   ├── parser.hpp             # Parser combinator library
│   ├── xstring.hpp            # Extended string utilities
│   └── grammars/
│       └── abasic.hpp          # BASIC grammar parser
├── examples/                   # Sample applications
│   ├── simple/
│   │   └── hello_world.cpp
│   ├── complex/
│   │   ├── main.cpp
│   │   ├── module_a.cpp
│   │   └── module_a.hpp
│   └── parser_examples.cpp
├── tests/                      # Test suite
│   ├── test_main.cpp          # Test entry point
│   ├── test_stlx.cpp          # STLX utilities tests
│   ├── test_parser.cpp        # Parser combinator tests
│   ├── test_xstring.cpp       # String utilities tests
│   ├── test_simple.cpp        # Simple utility tests
│   └── test_abasic.cpp        # BASIC grammar tests
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

**Note**: Only one sanitizer can be enabled at a time.

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
- `cpplint` - Run cpplint style checking (if available)
- `iwyu` - Run Include What You Use analysis (if available)
- `clang-tidy-report` - Run clang-tidy analysis (if available)
- `format` - Format code with clang-format (if available)
- `valgrind-tests` - Run tests with valgrind (if available)
- `generate-all-reports` - Generate all available analysis reports
- `report-summary` - Create summary of all analysis reports

### Coverage Targets
- `coverage_raw` - Generate raw coverage data
- `coverage_html` - Generate HTML coverage report (requires gcovr)
- `coverage` - Generate both raw and HTML reports

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `SANITIZER_TYPE` | `None` | Sanitizer type (`None`, `Address`, `UndefinedBehavior`, `Thread`, `Leak`, `Memory`) |
| `ENABLE_COVERAGE` | `OFF` | Enable code coverage with gcovr |
| `ENABLE_CLANG_TIDY` | `OFF` | Enable clang-tidy static analysis |
| `ENABLE_CPPCHECK` | `OFF` | Enable cppcheck static analysis |
| `ENABLE_CPPLINT` | `OFF` | Enable cpplint style checking |
| `ENABLE_IWYU` | `OFF` | Enable Include What You Use analysis |

### Building with All Options
```bash
mkdir build-full
cd build-full
cmake -DSANITIZER_TYPE=UndefinedBehavior \
      -DENABLE_COVERAGE=ON \
      -DENABLE_CLANG_TIDY=ON \
      -DENABLE_CPPCHECK=ON \
      ..
ninja
```

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

## Parser Combinator Library

STLX includes a comprehensive parser combinator library for building recursive descent parsers:

### Features
- **Character Parsing**: Single character and character range matching
- **String Matching**: Exact string and regex pattern matching
- **Combinators**: AND, OR, NOT logic combinators
- **Quantifiers**: Zero-or-more, one-or-more, zero-or-one
- **Whitespace Handling**: Automatic whitespace skipping
- **Error Reporting**: Detailed parse error information
- **AST Support**: Rule-based parsing with match counting

### Usage Example
```cpp
#include <stlx/parser.hpp>

using namespace stlx::parse;

// Define terminal rules
using digit = characters<std::string::iterator, '0', '9'>;
using plus = character<std::string::iterator, '+'>;

// Define parser with child rules
digit digit_parser;
one_or_more_<std::string::iterator, digit> number_parser(std::make_shared<digit>(digit_parser));

// Parse a string
std::string input = "123+456";
auto begin = input.begin();
auto end = input.end();
context<std::string::iterator> ctx(begin, end);

bool success = number_parser.parse(ctx, begin, end);
```

## BASIC Grammar Parser

STLX includes a complete BASIC-like grammar parser demonstrating complex parser construction:

### Features
- **Expressions**: Arithmetic operations with proper precedence
- **Variables**: LET assignments and INPUT statements
- **Control Flow**: IF/THEN/ELSE, FOR/WHILE loops
- **I/O**: PRINT and INPUT statements
- **Subroutines**: GOSUB/RETURN support
- **CRTP-based**: Proper type information for debugging

### Example Program
```basic
LET x = 10
LET y = 20
IF x > y THEN
  LET result = x
ELSE
  LET result = y
ENDIF
PRINT result
FOR i = 1 TO 5 STEP 1
  LET sum = sum + i
NEXT i
WHILE x < 100
  LET x = x * 2
WEND
END
```

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

## Generating Analysis Reports

All analysis reports are generated in the `build/reports/` directory:

```bash
cd build
ninja generate-all-reports  # Generate all analysis reports
ninja report-summary         # Create summary report

# Reports are available in:
ls reports/
# - cppcheck.xml
# - cpplint.txt
# - iwyu.txt
# - clang-format.txt
# - clang-tidy.txt
# - valgrind.xml
# - summary.txt
```

## Requirements

### Compiler
- GCC 11+ or Clang 14+ with C++23 support
- CMake 3.20+
- Ninja build system (recommended)

### Optional Tools
- **clang-tidy** - Static analysis
- **clang-format** - Code formatting
- **cppcheck** - Static analysis
- **cpplint** - Style checking
- **IWYU** - Include analysis
- **valgrind** - Dynamic analysis
- **gcovr** - Code coverage
- **Python 3** - For some analysis tools

## Contributing

1. Ensure code follows C++23 standards
2. Add tests for new functionality
3. Run static analysis tools before submitting
4. Format code with clang-format

## License

This project is provided as an educational example of modern C++23 development practices.
