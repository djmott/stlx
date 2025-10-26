# Investigate BASIC Parser Test Failures

## Current Status

- **Tests**: 60/62 passing (96.8%)
- **Failing Tests**: 
  - `ABasicGrammarTest.ParseCompleteProgram`
  - `ABasicGrammarTest.ParseComplexProgramWithAllFeatures`
- **Root Cause**: Unknown - need to investigate

## Problem Statement

Both failing tests parse multi-statement BASIC programs with:
- Variable declarations (LET statements)
- Conditional statements (IF...THEN...ELSE...ENDIF)
- Loops (FOR...NEXT, WHILE...WEND)
- Function calls (GOSUB, RETURN)
- End statement (END)

Earlier investigation showed error: `Expected=Expected character in range [A-Z], Found=x`

This suggests the parser expects uppercase keywords but identifiers can be lowercase.

## Investigation Tasks

### Task 1: Analyze ParseCompleteProgram Test Input

**Objective**: Understand what exact input is being parsed

**Approach**:
1. Read the test file `tests/test_abasic.cpp` around line 114-139
2. Examine the exact test input string
3. Identify pattern: are keywords uppercase but identifiers lowercase?

### Task 2: Test Individual Grammar Components

**Objective**: Determine which grammar component is failing

**Approach**:
1. Test parsing just "LET x = 10" 
2. Test parsing just " IF x > 5 THEN ... ENDIF "
3. Test parsing sequential LET statements
4. Test parsing complete program step by step
5. Record which step fails

### Task 3: Check Grammar Case Sensitivity

**Objective**: Verify if grammar expects exact case matching

**Approach**:
1. Check how keywords are defined (are they all uppercase?)
2. Check if identifiers have case requirements
3. Check if string literal parsing has issues
4. Check if whitespace handling is correct

### Task 4: Add Debug Output

**Objective**: Get detailed information about parsing failure

**Approach**:
1. Add debug prints to show:
   - Input being parsed
   - What rule is being tried
   - Where parsing stops
   - What error is generated
2. Run failing tests with debug output
3. Analyze the output to find root cause

### Task 5: Test Word Boundary Edge Cases

**Objective**: Verify word boundary checks aren't causing issues

**Approach**:
1. Test that "LET x = 10" parses correctly
2. Test that keywords followed by spaces work
3. Test that keywords followed by identifiers work
4. Check if word boundary check needs adjustment

## Diagnostic Commands

```bash
# Run failing tests with more output
./.build/test-base/bin/stlx_tests --gtest_filter="ABasicGrammarTest.ParseCompleteProgram"

# Create minimal test program
cd /tmp
# Test individual components step by step
```

## Expected Outcomes

1. **Identify** the exact grammar rule causing failure
2. **Understand** whether it's a case sensitivity issue, word boundary issue, or grammar structure issue
3. **Propose** concrete fix for the failing tests
4. **Validate** fix by running all tests

## Files to Examine

- `tests/test_abasic.cpp` (lines 114-139 for failing tests)
- `include/stlx/grammars/abasic.hpp` (grammar definitions)
- `include/stlx/parser.hpp` (word boundary check logic, lines 666-680)

## Success Criteria

- [ ] Both failing tests now pass
- [ ] Total test suite remains 62 tests
- [ ] No regressions in other tests

