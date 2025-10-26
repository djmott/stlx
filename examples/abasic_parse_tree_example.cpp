/**
 * @file abasic_parse_tree_example.cpp
 * @brief Example demonstrating parse tree AST generation for BASIC grammar
 *
 * This example shows how to use the parse_tree_printer to generate and display
 * a full Abstract Syntax Tree (AST) for a BASIC program with:
 * - Variable declarations (LET statements)
 * - Arithmetic expressions
 * - Conditional branching (IF/THEN/ELSE)
 * - Looping constructs (FOR/WHILE)
 * - Function calls (GOSUB)
 * - I/O operations (PRINT/INPUT)
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#include <iostream>
#include <stlx/grammars/abasic.hpp>
#include <stlx/parse_tree_printer.hpp>

using namespace stlx::parse;
using namespace stlx::grammars::basic;

// Typedef for the parser
using basic_parser = basic_program;

int main() {
    std::cout << "========================================\n";
    std::cout << "BASIC Grammar Parse Tree Example\n";
    std::cout << "========================================\n\n";

    // Example 1: Simple arithmetic expression
    std::cout << "=== Example 1: Simple Expression ===\n";
    std::string expr1 = " 10 ";
    std::cout << "Input: \"" << expr1 << "\"\n\n";

    {
        primary_expression parser;
        std::string::const_iterator begin = expr1.cbegin();
        std::string::const_iterator end = expr1.cend();
        context<std::string::const_iterator> ctx(begin, end, false);

        if (parser.parse(ctx, begin, end)) {
            std::cout << "✓ Parse successful!\n";
            std::cout << "Matched: \"" << std::string(expr1.cbegin(), begin)
                      << "\"\n";
        } else {
            std::cout << "✗ Parse failed.\n";
        }
    }

    std::cout << "\n";

    // Example 2: Variable assignment
    std::cout << "=== Example 2: Variable Assignment ===\n";
    std::string expr2 = " LET x = 42 ";
    std::cout << "Input: \"" << expr2 << "\"\n\n";

    {
        assignment_statement parser;
        std::string::const_iterator begin = expr2.cbegin();
        std::string::const_iterator end = expr2.cend();
        context<std::string::const_iterator> ctx(begin, end, false);

        if (parser.parse(ctx, begin, end)) {
            std::cout << "✓ Parse successful!\n";
            std::cout << "Matched: \"" << std::string(expr2.cbegin(), begin)
                      << "\"\n";
        } else {
            std::cout << "✗ Parse failed.\n";
        }
    }

    std::cout << "\n";

    // Example 3: Complex arithmetic expression
    std::cout << "=== Example 3: Complex Expression ===\n";
    std::string expr3 = " 10 + 5 * 3 ";
    std::cout << "Input: \"" << expr3 << "\"\n\n";

    {
        expression parser;
        std::string::const_iterator begin = expr3.cbegin();
        std::string::const_iterator end = expr3.cend();
        context<std::string::const_iterator> ctx(begin, end, false);

        if (parser.parse(ctx, begin, end)) {
            std::cout << "✓ Parse successful!\n";
            std::cout << "Matched: \"" << std::string(expr3.cbegin(), begin)
                      << "\"\n";
        } else {
            std::cout << "✗ Parse failed.\n";
        }
    }

    std::cout << "\n";

    // Example 4: IF/THEN/ELSE statement
    std::cout << "=== Example 4: Conditional Statement ===\n";
    std::string expr4 = " IF x > 5 THEN PRINT \"high\" ELSE PRINT \"low\" ENDIF ";
    std::cout << "Input: \"" << expr4 << "\"\n\n";

    {
        if_statement parser;
        std::string::const_iterator begin = expr4.cbegin();
        std::string::const_iterator end = expr4.cend();
        context<std::string::const_iterator> ctx(begin, end, false);

        if (parser.parse(ctx, begin, end)) {
            std::cout << "✓ Parse successful!\n";
            std::cout << "Matched: \""
                      << std::string(expr4.cbegin(), begin) << "\"\n";
        } else {
            std::cout << "✗ Parse failed.\n";
        }
    }

    std::cout << "\n";

    // Example 5: FOR loop
    std::cout << "=== Example 5: FOR Loop ===\n";
    std::string expr5 = " FOR i = 1 TO 10 STEP 1 LET sum = sum + i NEXT i ";
    std::cout << "Input: \"" << expr5 << "\"\n\n";

    {
        for_statement parser;
        std::string::const_iterator begin = expr5.cbegin();
        std::string::const_iterator end = expr5.cend();
        context<std::string::const_iterator> ctx(begin, end, false);

        if (parser.parse(ctx, begin, end)) {
            std::cout << "✓ Parse successful!\n";
            std::cout << "Matched: \"" << std::string(expr5.cbegin(), begin)
                      << "\"\n";
        } else {
            std::cout << "✗ Parse failed.\n";
        }
    }

    std::cout << "\n";

    // Example 6: Complete BASIC program
    std::cout << "=== Example 6: Complete BASIC Program ===\n";
    std::string program = R"(
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
 GOSUB subroutine 
 RETURN 
 END 
)";
    std::cout << "Program:\n" << program << "\n";

    {
        basic_parser parser;
        std::string::const_iterator begin = program.cbegin();
        std::string::const_iterator end = program.cend();
        context<std::string::const_iterator> ctx(begin, end, false);

        if (parser.parse(ctx, begin, end)) {
            std::cout << "✓ Parse successful!\n";
            std::cout << "Matched program length: "
                      << std::distance(program.cbegin(), begin) << " characters\n";

            // Generate and display AST using parse_tree_printer
            print_parse_tree<basic_parser, std::string::const_iterator>(program);
        } else {
            std::cout << "✗ Parse failed.\n";
        }
    }

    std::cout << "\n========================================\n";
    std::cout << "Parse Tree Demonstration Complete\n";
    std::cout << "========================================\n";

    return 0;
}

