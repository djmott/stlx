# Project Context

This file captures the current state of the STLX project, including recent changes, current issues, and development context.

## Current Project Status

**Date**: 2025
**Branch**: dev
**Tests**: 51/51 passing (100%)

## Recent Changes

### Removed Files
- `include/stlx/grammars/abasic.hpp` - BASIC grammar parser (removed)
- `tests/test_abasic.cpp` - BASIC grammar tests (removed, 11 tests)
- `examples/abasic_parse_tree_example.cpp` - BASIC parse tree example (removed)

**Commit**: `3890933` - Remove BASIC grammar

### Current Project Structure

```
stlx/
├── include/stlx/
│   ├── stlx.hpp               # Main utilities header
│   ├── parser.hpp             # Parser combinator library
│   ├── xstring.hpp            # Extended string utilities
│   └── parse_tree_printer.hpp # Parse tree visualization
├── examples/
│   ├── simple/hello_world.cpp
│   ├── complex/ (main.cpp, module_a.cpp, module_a.hpp)
│   ├── parser_examples.cpp
│   └── parse_tree_demo.cpp
└── tests/
    ├── test_main.cpp
    ├── test_stlx.cpp
    ├── test_parser.cpp
    ├── test_xstring.cpp
    └── test_simple.cpp
```

## Parser Combinator Library

The STLX parser combinator library (`include/stlx/parser.hpp`) provides:

### Features
- **Combinators**: `and_`, `or_`, `not_` with longest-match strategy
- **Quantifiers**: `zero_or_more_`, `one_or_more_`, `zero_or_one_`
- **Terminals**: `character`, `characters`, `string`, `regex`
- **EOF Handling**: `eof` rule with whitespace skipping
- **Word Boundaries**: Keywords cannot match as prefixes of other words
- **Static Parsing**: All parsing is static, instances created only after successful parse
- **CRTP Pattern**: Rules use CRTP for proper type introspection
- **Error Reporting**: Detailed parse error information

### Key Design Decisions

1. **Longest-Match Strategy**: The `or_` combinator tests all alternatives and selects the one that consumes the most input
2. **Word Boundary Checks**: The `string` parser checks that keywords are followed by non-alphanumeric characters
3. **Whitespace Skipping**: Automatic whitespace handling through `skip_ws` function
4. **Static Parsing**: All parsing logic is static - no instance methods

### Recent Fixes

1. **EOF Rule**: Modified to skip whitespace before checking end-of-input
2. **Longest-Match**: Implemented proper longest-match strategy in `or_` combinator
3. **Word Boundaries**: Added word boundary checking to `string` parser

## Core Library Features

### String Utilities (`include/stlx/xstring.hpp`)
- `trim()`, `trim_left()`, `trim_right()`
- `starts_with()`, `ends_with()`, `contains()`
- String view conversion utilities

### Main Utilities (`include/stlx/stlx.hpp`)
- Numeric utilities: `clamp()`, `in_range()`
- Format utilities: `format_string()`, `print_string()`, `print_line()`
- RAII utilities: `make_scope_exit()`
- Compile-time features: `hash_string()`, concepts

## CI/CD Status

### GitHub Actions
- ✅ CI/CD Pipeline (`ci.yml`)
- ✅ Pull Request Checks (`pr.yml`)
- ✅ Release Pipeline (`release.yml`)
- ✅ Scheduled Analysis (`scheduled.yml`)

### Analysis Tools
- **Static Analysis**: cppcheck, cpplint, clang-tidy, IWYU
- **Dynamic Analysis**: Valgrind, Address Sanitizer, UndefinedBehavior Sanitizer, Thread Sanitizer, Leak Sanitizer, Memory Sanitizer
- **Code Coverage**: gcovr with HTML reports
- **Code Formatting**: clang-format

## Build Configuration

### CMake Options
- `SANITIZER_TYPE`: None (default), Address, UndefinedBehavior, Thread, Leak, Memory
- `ENABLE_COVERAGE`: OFF by default
- `ENABLE_CLANG_TIDY`: OFF by default
- `ENABLE_CPPCHECK`: OFF by default
- `ENABLE_CPPLINT`: OFF by default
- `ENABLE_IWYU`: OFF by default

### Build Targets
- Examples: `hello_world`, `complex_example`, `parser_examples`, `parse_tree_demo`
- Tests: `stlx_tests`
- Analysis: `cppcheck`, `clang-tidy-report`, `valgrind-tests`, `generate-all-reports`

## Development Notes

### Removed Grammar
The BASIC grammar was removed due to:
- Complex multi-line IF statement parsing issues
- `comparison_expression` overmatching in multi-line contexts
- Investigation showed parser worked for single-line but not multi-line formats
- Decision made to move to C-style grammar instead

### Parser Performance
- All tests pass with good performance
- Parser uses static methods for efficiency
- Longest-match strategy ensures correct grammar selection

## Known Issues

None currently. All 51 tests passing.

## Next Steps

Ready to implement a C-style grammar to replace the BASIC grammar.

## Maintenance Notes

- Always rebuild after grammar changes: `cmake --build .build/test-base`
- Run tests to verify: `.build/test-base/bin/stlx_tests`
- Check for lint errors: `read_lints`
- Follow Codacy rules for automatic analysis after edits

