/**
 * @file parse_tree_printer.hpp
 * @brief AST generation and printing for parser combinator library
 *
 * This file provides functionality to build a runtime AST from parsed elements
 * and print the parse tree structure to stdout with proper indentation and
 * type information.
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef STLX_PARSE_TREE_PRINTER_HPP
#define STLX_PARSE_TREE_PRINTER_HPP

#include "stlx/parser.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

namespace stlx {
namespace parse {

// Template class for generating and printing AST
template <typename GrammarRule, typename Iterator = std::string::const_iterator>
class parse_tree_printer {
public:
    using iterator_type = Iterator;
    using grammar_type = GrammarRule;
    using ast_ptr = std::shared_ptr<grammar_type>;
    using rule_ptr = std::shared_ptr<rule_base<Iterator>>;

    parse_tree_printer() = default;

    // Print AST to stdout with indentation
    void print_ast(const rule_ptr& node, int indent = 0,
                    const std::string& prefix = "") {
        if (!node) {
            return;
        }

        // Get rule name and matched text
        std::string rule_name = node->name();
        std::string_view matched_text = node->get_text();

        // Print current node
        std::cout << prefix << "`- " << rule_name;

        // Print matched text if it's short enough
        if (!matched_text.empty() && matched_text.length() <= 60) {
            // Escape special characters for display
            std::string display_text(matched_text.begin(), matched_text.end());
            std::replace(display_text.begin(), display_text.end(), '\n', '\\');
            std::replace(display_text.begin(), display_text.end(), '\t', '\\');

            std::cout << " [" << display_text << "]";
        }

        std::cout << "\n";

        // Print children
        for (size_t i = 0; i < node->size(); ++i) {
            bool is_last = (i == node->size() - 1);
            std::string child_prefix = prefix + (is_last ? "    " : "|   ");
            print_ast((*node)[i], indent + 2, child_prefix);
        }
    }

    // Print AST in a compact format
    void print_ast_compact(const rule_ptr& node, int depth = 0) {
        if (!node) {
            return;
        }

        // Get rule name and matched text
        std::string rule_name = node->name();
        std::string_view matched_text = node->get_text();

        // Indentation
        std::string indent(depth * 2, ' ');

        // Print node
        std::cout << indent << rule_name;

        // Print matched text in quotes if short
        if (!matched_text.empty() && matched_text.length() <= 40) {
            std::string display_text(matched_text.begin(), matched_text.end());
            std::replace(display_text.begin(), display_text.end(), '\n', '\\');
            std::replace(display_text.begin(), display_text.end(), '\t', '\\');
            std::cout << ": \"" << display_text << "\"";
        }

        std::cout << "\n";

        // Print children
        for (size_t i = 0; i < node->size(); ++i) {
            print_ast_compact((*node)[i], depth + 1);
        }
    }

    // Print parse statistics
    void print_statistics(const rule_ptr& node) {
        if (!node) {
            std::cout << "No AST generated.\n";
            return;
        }

        std::string rule_name = node->name();
        std::string_view matched_text = node->get_text();

        std::cout << "=== Parse Tree Statistics ===\n";
        std::cout << "Total nodes: " << count_nodes(node) << "\n";
        std::cout << "Max depth: " << max_depth(node) << "\n";
        std::cout << "Root type: " << rule_name << "\n";
        std::cout << "Matched text length: " << matched_text.length()
                  << " characters\n";
        std::cout << "Children: " << node->size() << "\n";
        std::cout << "============================\n";
    }

private:
    [[nodiscard]] size_t count_nodes(const rule_ptr& node) const {
        if (!node) {
            return 0;
        }
        size_t count = 1;
        for (size_t i = 0; i < node->size(); ++i) {
            if ((*node)[i]) {
                count += count_nodes((*node)[i]);
            }
        }
        return count;
    }

    [[nodiscard]] size_t max_depth(const rule_ptr& node) const {
        if (!node) {
            return 0;
        }
        if (node->size() == 0) {
            return 1;
        }
        size_t max = 0;
        for (size_t i = 0; i < node->size(); ++i) {
            if ((*node)[i]) {
                size_t depth = max_depth((*node)[i]);
                if (depth > max) {
                    max = depth;
                }
            }
        }
        return max + 1;
    }
    
};

// Helper function to print a fully constructed AST
template <typename GrammarRule, typename Iterator = std::string::const_iterator>
void print_parse_tree(const std::shared_ptr<rule_base<Iterator>>& ast) {
    using printer_type = parse_tree_printer<GrammarRule, Iterator>;
    printer_type printer;
    
    if (ast) {
        std::cout << "\n=== Parse Tree ===\n\n";
        printer.print_ast(ast);
        std::cout << "\n=== Compact Format ===\n\n";
        printer.print_ast_compact(ast);
        std::cout << "\n";
        printer.print_statistics(ast);
    } else {
        std::cout << "No AST provided.\n";
    }
}

// Overload that parses from string and then prints
template <typename GrammarRule, typename Iterator = std::string::const_iterator>
void print_parse_tree(const std::string& input) {
    std::shared_ptr<GrammarRule> ast;
    
    if (parser<GrammarRule>::parse(input.cbegin(), input.cend(), ast)) {
        print_parse_tree<GrammarRule, Iterator>(std::static_pointer_cast<rule_base<Iterator>>(ast));
    } else {
        std::cout << "Parse failed - no AST generated.\n";
    }
}

}  // namespace parse
}  // namespace stlx

#endif  // STLX_PARSE_TREE_PRINTER_HPP

