# Sanitizer Configuration

## Overview

The STLX project uses a single sanitizer configuration system where only **ONE** sanitizer type can be enabled at a time. This ensures predictable behavior and prevents conflicting instrumentation.

## Available Sanitizer Types

1. **None** (default) - No sanitizer instrumentation
2. **Address** - Address Sanitizer (ASAN)
3. **UndefinedBehavior** - Undefined Behavior Sanitizer (UBSAN)
4. **Thread** - Thread Sanitizer (TSAN)
5. **Leak** - Leak Sanitizer (LSAN)
6. **Memory** - Memory Sanitizer (MSAN)

## Usage

### CMake Configuration

```bash
# Configure with a specific sanitizer
cmake -DSANITIZER_TYPE=Address -S . -B build
cmake -DSANITIZER_TYPE=UndefinedBehavior -S . -B build
cmake -DSANITIZER_TYPE=Thread -S . -B build
cmake -DSANITIZER_TYPE=Leak -S . -B build
cmake -DSANITIZER_TYPE=Memory -S . -B build

# No sanitizer (default)
cmake -DSANITIZER_TYPE=None -S . -B build
```

### Configuration Examples

```bash
# GCC with Address Sanitizer
cmake -S . -B build/gcc-asan -DSANITIZER_TYPE=Address \
      -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++

# Clang with Thread Sanitizer
cmake -S . -B build/clang-tsan -DSANITIZER_TYPE=Thread \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

# Undefined Behavior Sanitizer
cmake -S . -B build/ubsan -DSANITIZER_TYPE=UndefinedBehavior
```

## Important Notes

- ⚠️ **Only ONE sanitizer can be active at a time**
- The "All" option has been **removed** to prevent conflicts
- Each sanitizer type is mutually exclusive
- Use "None" to disable all sanitizers
- Sanitizer selection is validated and unknown types show a warning

## Implementation

The sanitizer logic is in `cmake_modules/Sanitizers.cmake`:

- Each sanitizer type is checked individually
- Compiler capability is verified before applying
- Runtime options are set for supported sanitizers
- Warnings are shown for unsupported types

## Runtime Options

Some sanitizers set runtime environment variables:

- **Address**: `ASAN_OPTIONS=abort_on_error=1`
- **UndefinedBehavior**: `UBSAN_OPTIONS=abort_on_error=1`

## Verification

To test all sanitizer types:

```bash
for sanitizer in None Address UndefinedBehavior Thread Leak Memory; do
    cmake -DSANITIZER_TYPE=$sanitizer -S . -B build/test-$sanitizer
done
```

