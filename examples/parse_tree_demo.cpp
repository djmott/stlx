// Copyright (c) 2025 David Mott
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <stlx/parser.hpp>
#include <stlx/parse_tree_printer.hpp>

using namespace stlx::parse;

// Simple number parser for demonstration
using digit = characters<std::string::const_iterator, '0', '9'>;
using number_literal = one_or_more_<std::string::const_iterator, digit>;

struct number_rule : rule<std::string::const_iterator, number_rule, number_literal> {};

int main() {
    std::cout << "=== Parse Tree Printer Demo ===\n\n";

    // Test with a simple number
    std::string input = "123";
    std::cout << "Input: \"" << input << "\"\n\n";

    number_rule parser;
    auto begin = input.cbegin();
    auto end = input.cend();
    context<std::string::const_iterator> ctx(begin, end, false);

    if (parser.parse(ctx, begin, end)) {
        std::cout << "Parse successful!\n";
        std::cout << "Matched: \"" << input << "\"\n";
        std::cout << "Rule type: " << typeid(parser).name() << "\n";
    } else {
        std::cout << "Parse failed.\n";
    }

    std::cout << "\n";
    
    // Create AST using the parser
    std::shared_ptr<number_rule> ast;
    if (stlx::parser<number_rule>::parse(input.cbegin(), input.cend(), ast)) {
        // Display the AST
        print_parse_tree<number_rule, std::string::const_iterator>(
            std::static_pointer_cast<rule_base<std::string::const_iterator>>(ast));
    } else {
        std::cout << "Could not generate AST for display.\n";
    }

    return 0;
}
