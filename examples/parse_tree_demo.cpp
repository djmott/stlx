// Copyright (c) 2025 David Mott
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <stlx/parser.hpp>
#include <stlx/parse_tree_printer.hpp>

using namespace stlx::parse;

// Simple number parser for demonstration
using digit = characters<std::string::iterator, '0', '9'>;
using number_literal = one_or_more_<std::string::iterator, digit>;

struct number_rule : rule<std::string::iterator, number_rule, number_literal> {};

int main() {
    std::cout << "=== Parse Tree Printer Demo ===\n\n";

    // Test with a simple number
    std::string input = "123";
    std::cout << "Input: \"" << input << "\"\n\n";

    number_rule parser;
    auto begin = input.begin();
    auto end = input.end();
    context<std::string::iterator> ctx(begin, end, false);

    if (parser.parse(ctx, begin, end)) {
        std::cout << "Parse successful!\n";
        std::cout << "Matched: \"" << input << "\"\n";
        std::cout << "Rule type: " << typeid(parser).name() << "\n";
    } else {
        std::cout << "Parse failed.\n";
    }

    std::cout << "\nNote: Full AST generation requires extended parser implementation.\n";
    std::cout << "The parse_tree_printer.hpp provides the framework for AST building.\n";

    return 0;
}
